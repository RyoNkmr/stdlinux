#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char const* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "%s: wrong argments\n", argv[0]);
    exit(1);
  }

  int fd;

  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    perror(argv[0]);
    exit(1);
  }

  if (unlink(argv[1]) < 0) {
    perror(argv[0]);
    exit(1);
  }


  if (close(fd) < 0) {
    perror(argv[0]);
    exit(1);
  }

  return 0;
}
