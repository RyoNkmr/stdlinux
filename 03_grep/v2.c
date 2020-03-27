#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#define _GNU_SOURCE
#include <getopt.h>

static struct option longopts[] = {
  { "ignore-case", no_argument, NULL, 'i' },
  { "invert-match", no_argument, NULL, 'v' },
  {0,0,0,0},
};

static void do_grep(regex_t *pat, FILE *f, int invert);

int main(int argc, char const* argv[])
{

  int opt;
  int cflags = REG_EXTENDED | REG_NOSUB | REG_NEWLINE;
  int invert = 0;

  while ((opt = getopt_long(argc, argv, "iv", longopts, NULL)) != -1) {
    switch (opt) {
      case 'i':
        cflags |= REG_ICASE;
        break;
      case 'v':
        invert = 1;
        break;
      case '?':
        fprintf(stderr, "Usage: %s [-iv] [FILE ...]\n", argv[0]);
        exit(1);
    }
  }

  if (argc < 2) {
    fputs("no pattern\n", stderr);
    exit(1);
  }

  regex_t pat;
  int err = regcomp(&pat, argv[optind], cflags);
  if (err != 0) {
    char buf[1024];
    regerror(err, &pat, buf, sizeof buf);
    puts(buf);
    exit(1);
  }

  if (optind == argc) {
    do_grep(&pat, stdin, invert);
    regfree(&pat);
    exit(0);
  }

  for(int i = optind + 1; i < argc; i++)  {
    FILE *f;
    f = fopen(argv[i], "r");
    if (!f) {
      perror(argv[i]);
      exit(1);
    }
    do_grep(&pat, f, invert);
    fclose(f);
  }

  regfree(&pat);
  exit(0);
}

static void do_grep(regex_t *pat, FILE *f, int invert) {
  char buf[4096];
  int match_with = invert == 1 ?  REG_NOMATCH : 0;

  while(fgets(buf, sizeof buf, f)) {
    if (regexec(pat, buf, 0, NULL, 0) == match_with) {
      fputs(buf, stdout);
    }
  }
}
