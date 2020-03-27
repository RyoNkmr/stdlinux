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
    int l = 0;
    while ((c = fgetc(f)) != EOF) {
      if (c == '\n') l++;
    }
    fclose(f);

    if (printf("%d", l) < 0) {
      exit(1);
    }
  }
  exit(0);
}
