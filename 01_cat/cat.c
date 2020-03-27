#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const* argv[])
{
  int i;

  for (i = 1; i < argc; i++) {
    FILE *f;
    f = fopen(argv[i], "r");

    if (!f) {
      perror(argv[i]);
      exit(1);
    }

    int c;
    while ((c = fgetc(f)) != EOF) {
      int r;
      int errno = 0;

      switch (c) {
        case '\t':
          r = fputs("\\t", stdout);
          break;
        case '\n':
          r = fputs("$\n", stdout);
          break;
        default:
          r = putchar(c);
      }

      if (r < 0 || errno != 0) {
        exit(1);
      }
    }
    fclose(f);
  }
  exit(0);
}

