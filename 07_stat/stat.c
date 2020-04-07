#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

static char *filetype(mode_t mode);

int main(int argc, char const* argv[])
{
  struct stat st;

  if (argc != 2) {
    fprintf(stderr, "%s: wrong argments\n", argv[0]);
    exit(1);
  }

  if (lstat(argv[1], &st) < 0) {
    perror(argv[1]);
    exit(1);
  }

  printf("type\t%o (%s)\n", (st.st_mode & S_IFMT), filetype(st.st_mode));
  printf("mode\t%o\n", st.st_mode & ~S_IFMT);
  printf("dev\t%llu\n", (unsigned long long)st.st_dev);
  printf("ino\t%lu\n", (unsigned long)st.st_ino);
  printf("rdev\t%llu\n", (unsigned long long)st.st_rdev);
  printf("nlink\t%lu\n", (unsigned long)st.st_nlink);
  printf("uid\t%d\n", st.st_uid);
  printf("gid\t%d\n", st.st_gid);
  printf("size\t%lld\n", st.st_size);
  printf("blksize\t%lu\n", (unsigned long)st.st_blksize);
  printf("blocks\t%lu\n", (unsigned long)st.st_blocks);
  printf("atime\t%s", ctime(&st.st_atime));
  printf("mtime\t%s", ctime(&st.st_mtime));
  printf("ctime\t%s", ctime(&st.st_ctime));
  return 0;
}

static char* filetype(mode_t mode) {
  switch (mode & S_IFMT) {
    case S_IFREG: return "file";
    case S_IFDIR: return "directory";
    case S_IFCHR: return "chardev";
    case S_IFBLK: return "blockdev";
    case S_IFIFO:  return "fifo";
    case S_IFLNK: return "symlink";
    case S_IFSOCK:  return "socket";
    default: return "unknown";
  }
}
