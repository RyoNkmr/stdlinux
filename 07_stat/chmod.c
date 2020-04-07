#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char const* argv[])
{
  if (argc < 2) {
    fprintf(stderr, "%s: no mode given\n", argv[0]);
    exit(1);
  }

  int mode = strtol(argv[1], NULL, 8);
  int i;
  for (int i = 2; i < argc; i++) {
    if (chmod(argv[i], mode) < 0) {
      perror(argv[i]);
    }
  }

  return 0;
}
