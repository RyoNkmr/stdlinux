#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

#define _GNU_SOURCE
#include <getopt.h>

#define ERR_TEMPLATE "Usage: %s [-a] [FILE ...]\n"

static struct option longopts[] = {
  { "all", no_argument, NULL, 'a' },
  {0,0,0,0},
};

static void do_tree(char *path, int depth, int show_dotfiles);
static void ppath(char *path, int depth);

int main(int argc, char const* argv[])
{
  int opt;
  int show_dotfiles;

  while ((opt = getopt_long(argc, argv, "a", longopts, NULL)) != -1) {
    switch (opt) {
      case 'a': 
        show_dotfiles = 1;
        break;
      case 'h':
        fprintf(stdout, ERR_TEMPLATE, argv[0]);
        exit(0);
      case '?':
        fprintf(stderr, ERR_TEMPLATE, argv[0]);
        exit(1);
    }
  }

  if (argc < 2) {
    do_tree(".", 0, show_dotfiles);
    return 0;
  }

  for(int i = 1; i < argc; i++) {
    do_tree((char *)argv[i], 0, show_dotfiles);
  }

  return 0;
}

static void ppath(char *path, int depth) {
  for (int i = 0; i < depth; i++) {
    putchar('-');
    putchar('-');
  }
  printf(" %s\n", path);
}


static void do_tree(char *path, int depth, int show_dotfiles) {
  struct stat st;

  if (lstat(path, &st) < 0) {
    return;
  }

  if (S_ISDIR(st.st_mode) == 0) {
    return ppath(path, depth);
  }

  if (strncmp(path, ".", 1) != 0) {
    ppath(path, depth);
  }

  struct dirent **namelist;

  int n = scandir(path, &namelist, NULL, alphasort);
  if (n < 0) {
    perror(path);
    exit(1);
  }


  for (int i = 0; i < n; i++) {
    char * dname = namelist[i]->d_name;
    if (show_dotfiles == 1) {
      if (strncmp(dname, ".", 2) == 0 || strncmp(dname, "..", 3) == 0) {
        continue;
      }
    } else {
      if (strncmp(dname, ".", 1) == 0) {
        continue;
      }
    }

    char pathname[strlen(path)+strlen(dname)+2];
    strcpy(pathname, path);
    strcat(pathname, "/");
    strcat(pathname, dname);
    do_tree(pathname, depth+1, show_dotfiles);
  }

  free(namelist);
}
