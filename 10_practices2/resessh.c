#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define CMD_SIZE 8
#define ARGV_SIZE 32
#define CHUNK_SIZE 256

#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct {
 char **argv;
 int argc;
} cmd;

static void read_stdin(void);
static void pwd(int is_fullpath, int brackets);
static void print_re(void);
static char* get_cwd(char *dest);
static int exec_cmds(int cmdc, cmd *cmds);
static void exec(int argc, char *argv[]);

static void bye(void);
static void cd(char *path);

static char *env_home;
static int env_home_len;

int main(int argc, char const* argv[])
{

  env_home = getenv("HOME");
  if (env_home != NULL) {
    env_home_len = strlen(env_home);
  }

  while(1) {
    puts("");
    pwd(1, 1);
    print_re();
    read_stdin();
  }
  return 0;
}

static void read_stdin(void) {
  cmd *cmds;
  int cmd_size = CMD_SIZE;
  cmds = (cmd *)calloc(cmd_size, sizeof(cmd));
  int cmdc = 0;

  int argv_size = ARGV_SIZE;
  char **argv;
  argv = (char **)calloc(argv_size, sizeof(char));
  int argc = 0;

  int chunk_size = CHUNK_SIZE;
  int chunk_ptr_index = 0;
  char *chunk;
  chunk = (char *)calloc(chunk_size, sizeof(char));

  int ch;

  while(1) {
    ch = getchar();
    if (ch == EOF) {
      perror("read_stdin");
      exit(1);
    }

    if (ch == ' ') {
      if (chunk_ptr_index == 0 || chunk[chunk_ptr_index] == ' ') {
        continue;
      }
    }

    if (ch == ' ' || ch == '\n') {
      if (argc > argv_size - 1) {
        argv_size *= 2;
        char **_argv;
        _argv = (char **)realloc(argv, sizeof(char *) * argv_size);
        if (_argv == NULL) {
          free(argv);
          perror("realloc argv");
          exit(1);
        }
        if (argv != _argv) {
          argv = _argv;
        }
      }

      int len = strlen(chunk);
      argv[argc] = calloc(len + 1, sizeof(char));
      strncpy(argv[argc], chunk, len);
      argc++;
    }

    if (ch == ' ') {
      free(chunk);
      chunk = (char *)calloc(chunk_size, sizeof(char));
      chunk_ptr_index = 0;
      continue;
    }

    if (ch == '|' || ch == '\n') {
      cmds[cmdc].argv = argv;
      cmds[cmdc].argc = argc;
      cmdc++;

      argv_size = ARGV_SIZE;
      argv = (char **)calloc(argv_size, sizeof(char));
      argc = 0;

      free(chunk);
      chunk_size = CHUNK_SIZE;
      chunk = (char *)calloc(chunk_size, sizeof(char));
      chunk_ptr_index = 0;
    }

    if (ch == '|') {
      continue;
    }

    if (ch == '\n') {
      exec_cmds(cmdc, cmds);
      break;
    }

    if (chunk_ptr_index > chunk_size - 1) {
      chunk_size *= 2;
      char *_chunk;
      _chunk = (char *)realloc(chunk, sizeof(char) * chunk_size);

      if (_chunk == NULL) {
        free(chunk);
        perror("realloc");
        exit(1);
      }
      if (chunk != _chunk) {
        chunk = _chunk;
      }
    }
    chunk[chunk_ptr_index] = ch;
    chunk_ptr_index++;
  }

  for (int i = 0; i < cmdc; i++) {
    for (int j = 0; j < cmds[i].argc; j++) {
      if (cmds[i].argv[j] == NULL) {
        break;
      }
      free(cmds[i].argv[j]);
    }
    free(cmds[i].argv);
  }
  free(cmds);
  free(chunk);
}

static int exec_cmds(int cmdc, cmd* cmds) {
  char **argv = cmds[0].argv;
  int argc = cmds[0].argc;
  pid_t *pids = (pid_t *)calloc(cmdc, sizeof(pid_t));

  if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "bye") == 0) {
    bye();
  }

  int original_stdinout[2];
  original_stdinout[PIPE_READ] = dup(STDIN_FILENO);
  original_stdinout[PIPE_WRITE] = dup(STDOUT_FILENO);

  int prev_pipe[2];
  if (pipe(prev_pipe) < 0) {
    perror("pipe");
    exit(1);
  }
  int current_pipe[2] = {-1, -1};

  for (int i = 0; i < cmdc; i++) {
    if (pipe(current_pipe) < 0) {
      perror("pipe");
      close(prev_pipe[PIPE_READ]);
      close(prev_pipe[PIPE_WRITE]);
      exit(1);
    }

    pids[i] = fork();

    if (pids[i] < 0) {
      fprintf(stderr, "fork(2) failed\n");
      close(prev_pipe[PIPE_READ]);
      close(prev_pipe[PIPE_WRITE]);
      close(current_pipe[PIPE_READ]);
      close(current_pipe[PIPE_WRITE]);
      _exit(1);
    }

    if (pids[i] == 0) {
      /* child */
      if (i > 0) {
        close(STDIN_FILENO);
        dup2(prev_pipe[PIPE_READ], STDIN_FILENO);
        close(prev_pipe[PIPE_READ]);
        close(prev_pipe[PIPE_WRITE]);
      }

      if (i < cmdc - 1) {
        close(current_pipe[PIPE_READ]);
        close(STDOUT_FILENO);
        dup2(current_pipe[PIPE_WRITE], STDOUT_FILENO);
        close(current_pipe[PIPE_WRITE]);
      }

      exec(cmds[i].argc, cmds[i].argv);
      perror("exec");
      _exit(99);
    }

    /* parent */
    close(prev_pipe[PIPE_READ]);
    close(prev_pipe[PIPE_WRITE]);
    if (i > 0) {
      close(current_pipe[PIPE_WRITE]);
      close(current_pipe[PIPE_READ]);
    }

    prev_pipe[PIPE_READ] = current_pipe[PIPE_READ];
    prev_pipe[PIPE_WRITE] = current_pipe[PIPE_WRITE];
  }

  close(STDIN_FILENO);
  dup2(original_stdinout[PIPE_READ], STDIN_FILENO);
  close(original_stdinout[PIPE_READ]);
  close(STDOUT_FILENO);
  dup2(original_stdinout[PIPE_WRITE], STDOUT_FILENO);
  close(original_stdinout[PIPE_WRITE]);

  int status;
  for (int i = 0; i < cmdc; i++) {
    waitpid(pids[i], &status, 0);
    if (!WIFEXITED(status) && !WIFSIGNALED(status)) {
      return -1;
    }
  }

  return 0;
}

static void exec(int argc, char* argv[]) {
  if (strcmp(argv[0], "pwd") == 0) {
    if (argc > 1) {
      fprintf(stderr, "%s: too many argments\n", argv[0]);
      _exit(1);
    }
    pwd(0, 0);
    _exit(0);
  }

  if (strcmp(argv[0], "cd") == 0) {
    if (argc != 2) {
      fprintf(stderr, "%s: wrong argments\n", argv[0]);
      _exit(1);
    }
    cd(argv[1]);
    _exit(0);
  }

  execvp(argv[0], argv);
  perror(argv[0]);
  _exit(99);
}

static void bye() {
  printf("resessh says goodbye to you👋\n");
  exit(0);
}

static char* _getcwd() {
  static int size = 256;
  char *ptr;
  ptr = calloc(size, sizeof(char));

  while(1) {
    if (getcwd(ptr, size) == NULL) {
      if (errno == ERANGE) {
        size *= 2;
        char *_ptr;
        _ptr = realloc(ptr, size * sizeof(char));

        if (_ptr == NULL) {
          perror("getcwd");
          free(ptr);
          exit(1);
        }

        if (ptr != _ptr) {
          ptr = _ptr;
        }
      }
      perror("getcwd");
      free(ptr);
      exit(1);
    }
    break;
  }

  return ptr;
}

static void pwd(int is_fullpath, int brackets) {
  char *name = _getcwd();
  if (env_home != NULL && !is_fullpath) {
    char *ptr;
    if ((ptr = strstr(name, env_home)) == name) {
      int len = strlen(ptr) - env_home_len + 1;

      char *_name;
      _name = calloc(len, sizeof(char));
      _name[0] = '~';
      for (int i = 1; i < len; i++) {
        _name[i] = name[env_home_len + i - 1];
      }
      free(name);
      name = _name;
    }
  }
  printf(brackets ?  "[%s]\n" : "%s\n", name);
  free(name);
}

static void print_re() {
  printf("李: ");
}

static void cd(char *path) {
  if (chdir(path) < 0) {
    perror("cd");
    return;
  };
}
