#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

static void do_tree(char *path, int depth);
static void ppath(char *path, int depth);

int main(int argc, char const* argv[])
{
  int i;
  if (argc < 2) {
    do_tree(".", 0);
    return 0;
  }

  for(i = 1; i < argc; i++) {
    do_tree((char *)argv[i], 0);
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


static void do_tree(char *path, int depth) {
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

  DIR  *d;
  struct dirent *entry;

  d = opendir(path);
  if (!d) {
    perror(path);
    exit(1);
  }

  while((entry = readdir(d))) {
    if (strncmp(entry->d_name, ".", 1) == 0) {
      continue;
    }
    char pathname[1024];
    strcpy(pathname, path);
    strcat(pathname, "/");
    strcat(pathname, entry->d_name);
    do_tree(pathname, depth+1);
  }

  closedir(d);
}
