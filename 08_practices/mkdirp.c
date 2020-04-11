#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

static void rec_mkdir(const char *path);

int main(int argc, char const* argv[])
{
  if (argc != 2) {
    fprintf(stderr, "%s: wrong argments\n", argv[0]);
    exit(1);
  }

  rec_mkdir(argv[1]);
  return 0;
}

static void rec_mkdir(const char *path) {
  if(mkdir(path, 0777) == 0 || errno == EEXIST) {
    return;
  }

  if (errno != ENOENT) {
    perror(path);
    exit(1);
  }

  char *ptr_trailing_slash = strrchr(path, '/');
  if (ptr_trailing_slash != NULL) {
    int len = ptr_trailing_slash - path;
    if(len == 0) {
      return;
    }

    char parent_path[len+1];
    strncpy(parent_path, path, len);
    parent_path[len] = '\0';

    rec_mkdir(parent_path);
  }

  if (mkdir(path, 0777) < 0) {
    perror(path);
    exit(1);
  }
}
