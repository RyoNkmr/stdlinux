#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

static void do_ls(char *path);

int main(int argc, char const* argv[])
{
  int i;
  if (argc < 2) {
    do_ls(".");
    return 0;
  }

  for(i = 1; i < argc; i++) {
    do_ls(argv[i]);
  }

  return 0;
}

static void do_ls(char *path) {
  DIR  *d;
  struct dirent *entry;

  d = opendir(path);
  if (!d) {
    perror(path);
    exit(1);
  }

  while(entry = readdir(d)) {
    printf("%s\n", entry->d_name);
  }
  closedir(d);
}
