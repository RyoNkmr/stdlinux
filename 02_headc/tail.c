#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

static void do_tail(FILE *f, long nlines);

#define DEFUALT_N_LINES 10

static struct option longopts[] = {
  { "lines", required_argument, NULL, 'n' },
  { "help", no_argument, NULL, 'h' },
  {0,0,0,0},
};

int main(int argc, char const* argv[])
{

  int opt;
  long nlines = DEFUALT_N_LINES;

  while ((opt = getopt_long(argc, argv, "n:", longopts, NULL)) != -1) {
    switch (opt) {
      case 'n':
        nlines = atol(optarg);
        if (nlines < 1) {
          fprintf(stderr, "%s: illegal line count -- %s\n", argv[0], optarg);
          exit(1);
        };
        break;
      case 'h':
        fprintf(stdout, "Usage: %s [-n LINES] [FILE ...]\n", argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, "Usage: %s [-n LINES] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  if (optind == argc) {
    do_tail(stdin, nlines);
    exit(0);
  }

  for (int i = optind; i < argc; i++) {
    FILE *f;
    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }
    do_tail(f, nlines);
    fclose(f);
  }

  exit(0);
}

static void do_tail(FILE *f, long nlines) {
  int offsets[nlines + 1];
  int oi = 0;
  int c;
  while((c = getc(f)) != EOF) {
    if (c == '\n') {
      offsets[oi] = ftello(f);
      oi = oi == nlines ? 0 : oi+1;
    }
  }

  fseeko(f, offsets[oi], SEEK_SET);
  while((c = getc(f)) != EOF) {
    if (putchar(c) < 0) exit(1);
  }
}
