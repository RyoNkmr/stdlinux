#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char const* argv[])
{
  if (argc != 3) {
    fprintf(stderr, "%s: wrong argments\n", argv[0]);
    exit(1);
  }

  int fd;

  if ((fd = open(argv[1], O_RDWR)) < 0) {
    perror(argv[0]);
    exit(1);
  }

  if (rename(argv[1], argv[2]) < 0) {
    perror(argv[0]);
    exit(1);
  }


  if (close(fd) < 0) {
    perror(argv[0]);
    exit(1);
  }

  return 0;
}
