#include <stdio.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
  char *const largv[] = {
    "ls",
    ".",
    NULL
  };
  execvp("ls", largv);
  perror(largv[0]);
  return -1;
}
