#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char const* argv[])
{
  if (argc < 2) {
    fprintf(stderr, "%s: wrong argments\n", argv[0]);
    exit(1);
  }

  char **args;
  args = calloc(argc, sizeof(const char*));

  for (int i = 1; i < argc; i++) {
    size_t len = strlen(argv[i]);
    args[i-1] = (char *)malloc(len + 1);
    strncpy(args[i-1], argv[i], len);
  }

  pid_t pid = fork();

  if (pid < 0) {
    fprintf(stderr, "fork(2) failed\n");
    for (int i = 0; i < argc; i++) {
      free(args[i]);
    }
    free(args);
    exit(1);
  }

  if (pid == 0) {
    execvp(args[0], args);
    perror(args[0]);
    exit(99);
  }

  int status;
  waitpid(pid, &status, 0);
  printf("child (PID=%d) exited;", pid);

  if (WIFEXITED(status)) {
    printf("exit, status=%d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("signal, signal=%d\n", WTERMSIG(status));
  } else {
    printf("abnormal exit\n");
  }

  for (int i = 0; i < argc; i++) {
    free(args[i]);
  }
  free(args);

  return 0;
}
