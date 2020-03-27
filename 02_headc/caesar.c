#include <stdio.h>
#include <stdlib.h>

#define _GNU_SOURCE
#include <getopt.h>

static void do_caesar(FILE *f, long nlines);

#define DEFUALT_N_SHIFT 1

static struct option longopts[] = {
  { "lines", required_argument, NULL, 'n' },
  { "help", no_argument, NULL, 'h' },
  {0,0,0,0},
};

int main(int argc, char const* argv[])
{

  int opt;
  long nshift = DEFUALT_N_SHIFT;

  while ((opt = getopt_long(argc, argv, "n:", longopts, NULL)) != -1) {
    switch (opt) {
      case 'n':
        nshift = atol(optarg);
        break;
      case 'h':
        fprintf(stdout, "Usage: %s [-n SHIFT] [FILE ...]\n", argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, "Usage: %s [-n SHIFT] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  if (optind == argc) {
    do_caesar(stdin, nshift);
    exit(0);
  }

  for (int i = optind; i < argc; i++) {
    FILE *f;
    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }
    do_caesar(f, nshift);
    fclose(f);
  }

  exit(0);
}

static void do_caesar(FILE *f, long nshift) {
  int c;
  while((c = getc(f)) != EOF) {
    if (c >= 'a' && c <= 'z') {
      c = (c - 'a' + nshift) % 26 + 'a';
    }
    if (c >= 'A' && c <= 'Z') {
      c = (c - 'A' + nshift) % 26 + 'A';
    }
    if (putchar(c) < 0) exit(1);
  }
}
