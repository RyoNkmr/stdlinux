#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 2048

int main(int argc, char const* argv[])
{
  int i;
  unsigned char buf[BUFFER_SIZE];

  for (i = 1; i < argc; i++) {
    FILE *f;
    int c;
    f = fopen(argv[i], "r");

    if (!f) {
      perror(argv[i]);
      exit(1);
    }

    while (!feof(f)) {
      c = fread(buf, 1, sizeof buf - 1, f);
      if (ferror(f)) {
        exit(1);
      }
      buf[c] = '\n';

      int errno = 0;
      fwrite(buf, 1, c, stdout);
      if (errno != 0) {
        exit(1);
      }
    }
    fclose(f);
  }
  exit(0);
}
