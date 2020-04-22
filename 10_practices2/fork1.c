#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
  int *buf;
  int len = 1000000000;
  buf = (int *)calloc(len, sizeof(int));
  for (int i = 0; i < len; i++) {
    buf[i] = i;
  }

  /* pid_t pid = fork(); */
  /* if (pid == 0) { */
    buf[0] = 999;
  /* } */
  printf("*buf: %p, buf[0]: %d\n", buf, buf[0]);
  sleep(100);
  free(buf);
  return 0;
}
