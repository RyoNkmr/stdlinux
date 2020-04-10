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
  if(mkdir(path, 0777) == 0) {
    return;
  }

  if (errno != ENOENT) {
    perror(path);
    exit(1);
  }

  char *ptr_trailing_slash = strrchr(path, '/');
  if (ptr_trailing_slash != NULL) {
    int len = ptr_trailing_slash - path;
    char parent_path[len+1];
    stpncpy(parent_path, path, len);
    parent_path[sizeof(parent_path) - 1] = '\0';
    printf("%s -> %s: (+%s) %d\n", path, parent_path, ptr_trailing_slash, len);
    if(strlen(path) == 0) {
      return;
    }
    rec_mkdir(parent_path);
  }
}
