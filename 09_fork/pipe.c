#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const* argv[])
{
  int pipefds[2];

  if (pipe(pipefds) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  if (pid == 0) {
    close(pipefds[1]);

    unsigned char buf[2048];
    while (1) {
      int bytes = read(pipefds[0], &buf, sizeof buf);
      if (bytes < 0) {
        perror("read");
        exit(1);
      }
      if (bytes == 0) {
        break;
      }
      write(STDOUT_FILENO, &buf, bytes);
    }
    write(STDOUT_FILENO, "\n", 1);
    close(pipefds[0]);
    exit(0);
  }

  close(pipefds[0]);
  char messages[][5] = {"hoge", "fuga", "piyo"};
  for (int i = 0; i < 3; i++) {
    write(pipefds[1], messages[i], 5);
  }
  close(pipefds[1]);

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
  exit(0);
}
