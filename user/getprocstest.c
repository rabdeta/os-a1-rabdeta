#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/uproc.h"
#include "user/user.h"

int
main(void)
{
  struct uproc procs[NPROC];
  int n;

  n = getprocs(procs, 1);
  printf("max=1: returned %d\n", n);

  n = getprocs(procs, 0);
  printf("max=0: returned %d\n", n);

  n = getprocs(procs, -1);
  printf("max=-1: returned %d\n", n);

  n = getprocs(procs, NPROC);
  printf("max=64: returned %d\n", n);

  n = getprocs((struct uproc *)0, 1);
  printf("invalid pointer: returned %d\n", n);

  exit(0);
}
