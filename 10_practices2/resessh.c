#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define ARGV_SIZE 256
#define CHUNK_SIZE 256

static void read_stdin(void);
static void print_cwd(void);
static char* get_cwd(char *dest);
static int exec(int argc, char* argv[]);
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
    print_cwd();
    read_stdin();
  }
  return 0;
}

static void read_stdin(void) {
  int argv_size = ARGV_SIZE;
  char **argv;
  argv = (char **)calloc(argv_size, sizeof(char));
  int argc = 0;
  int ch;

  int chunk_size = CHUNK_SIZE;
  int chunk_ptr_index = 0;
  char *chunk;
  chunk = (char *)calloc(chunk_size, sizeof(char));

  while(1) {
    ch = getchar();
    if (ch == EOF) {
      perror("read_stdin");
      exit(1);
    }

    if (ch == ' ' && chunk[chunk_ptr_index] == ' ') {
      continue;
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

    if (ch == '\n') {
      exec(argc, argv);
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

  for (int i = 0; i < argv_size ; i++) {
    if (argv[i] == NULL) {
      break;
    }
    free(argv[i]);
  }
  free(argv);
  free(chunk);
}

static int exec(int argc, char* argv[]) {
  if (strcmp(argv[0], "cd") == 0) {
    if (argc != 2) {
      fprintf(stderr, "%s: wrong argments\n", argv[0]);
      exit(1);
    }
    cd(argv[1]);
    return 0;
  }

  if (strcmp(argv[0], "exit") == 0) {
    printf("resessh says goodbye to you👋\n");
    exit(0);
  }

  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork(2) failed\n");
    exit(1);
  }

  if (pid == 0) {
    execvp(argv[0], argv);
    perror(argv[0]);
    exit(99);
  }

  int status;
  waitpid(pid, &status, 0);
  /* printf("child (PID=%d) exited;", pid); */
  /*  */
  /* if (WIFEXITED(status)) { */
  /*   printf("exit, status=%d\n", WEXITSTATUS(status)); */
  /* } else if (WIFSIGNALED(status)) { */
  /*   printf("signal, signal=%d\n", WTERMSIG(status)); */
  /* } else { */
  /*   printf("abnormal exit\n"); */
  /* } */
  /*  */
  return 0;
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

static void print_cwd() {
  char *name = _getcwd();

  if (env_home != NULL) {
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

  printf("[%s]\n李: ", name);
  free(name);
}

static void cd(char *path) {
  if (chdir(path) < 0) {
    perror("cd");
    return;
  };
}
