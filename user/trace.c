
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: trace mask command [args...]\n");
    exit(1);
  }

  // Convert the mask argument to an integer.
  int mask = atoi(argv[1]);

  // Enable tracing for this process.
  if (trace(mask) < 0) {
    fprintf(2, "trace: system call failed\n");
    exit(1);
  }

  // Replace this program with the requested command.
  exec(argv[2], &argv[2]);

  // Only reached if exec fails.
  fprintf(2, "trace: exec %s failed\n", argv[2]);
  exit(1);
}
