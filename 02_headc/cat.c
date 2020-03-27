#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

static void do_cat(FILE *f, int show_whitespaces);

#define DEFUALT_N_LINES 10

static struct option longopts[] = {
  { "show-whitespaces", no_argument, NULL, 'w' },
  { "help", no_argument, NULL, 'h' },
  {0,0,0,0},
};

int main(int argc, char const* argv[])
{

  int opt;
  int show_whitespaces = 0;

  while ((opt = getopt_long(argc, argv, "w", longopts, NULL)) != -1) {
    switch (opt) {
      case 'w':
        show_whitespaces = 1;
        break;
      case 'h':
        fprintf(stdout, "Usage: %s [-w] [FILE ...]\n", argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, "Usage: %s [-w] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  if (optind == argc) {
    do_cat(stdin, show_whitespaces);
    exit(0);
  }

  for (int i = optind; i < argc; i++) {
    FILE *f;
    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }
    do_cat(f, show_whitespaces);
    fclose(f);
  }

  exit(0);
}

static void do_cat(FILE *f, int show_whitespaces) {
  int c;
  while((c = getc(f)) != EOF) {
    if (show_whitespaces) {
      switch(c) {
        case '\t':
          if (puts('\\') < 0) exit(1);
          c = '\\t';
          break;
        case '\n':
          if (putchar('$') < 0) exit(1);
          break;
      }
    }
    if (putchar(c) < 0) exit(1);
  }
}
