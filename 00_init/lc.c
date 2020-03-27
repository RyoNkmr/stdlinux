#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static void count(const char *path);
static void die(const char *s);

int main(int argc, char const* argv[])
{
  int i;
  if (argc < 2) {
    fprintf(stderr, "%s: file name not given\n", argv[0]);
    exit(0);
  }

  for (int i = 1; i < argc; i++) {
    count(argv[i]);
  }
  exit(0);
}

#define BUFFER_SIZE 2048

static void count(const char *path) {
  int fd;
  unsigned char buf[BUFFER_SIZE];
  unsigned long count = 0;
  int n;

  fd = open(path, O_RDONLY);
  if (fd < 0) die(path);

  for(;;) {
    n = read(fd, buf, sizeof buf);
    if (n < 0) die(path);
    if (n == 0) break;

    for (int i = 0; i < sizeof buf; i++) {
      if (buf[i] == '\n') {
        count++;
      }
    }
  }
  if (close(fd) < 0) die(path);
  printf("%s: %lu\n", path, count);
}

static void die(const char *s) {
  perror(s);
  exit(1);
}
