#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define _GNU_SOURCE
#include <getopt.h>

static struct option longopts[] = {
  { "symbolic", no_argument, NULL, 's' },
  {0,0,0,0},
};

#define ERR_TEMPLATE "Usage: %s [-s] [FILE ...]\n"

int main(int argc, char const* argv[])
{
  int opt;

  while ((opt = getopt_long(argc, argv, "s", longopts, NULL)) != -1) {
    switch (opt) {
      case 's': {
                  if (symlink(argv[1], argv[2]) < 0) {
                    perror(argv[1]);
                    return 1;
                  }
                  return 0;
                }
      case 'h':
        fprintf(stdout, ERR_TEMPLATE, argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, ERR_TEMPLATE, argv[0]);
        exit(1);
    }
  }

  if (argc != 3) {
    fprintf(stderr, "%s: wrong arguments\n", argv[0]);
    return 1;
  }

  if (link(argv[1], argv[2]) < 0) {
    perror(argv[1]);
    return 1;
  }
  return 0;
}
