#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
  int i;

  if (argc < 2) {
    fprintf(stderr, "%s: no argments\n", argv[0]);
    return 1;
  }

  for (int i = 0; i < argc; i++) {
    if (unlink(argv[i])< 0) {
      perror(argv[i]);
      return 1;
    }
  }

  return 0;
}
