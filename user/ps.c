#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/uproc.h"
#include "user/user.h"

int
main(void)
{
  struct uproc procs[NPROC];
  char *states[] = {
    "unused",
    "used",
    "sleeping",
    "runnable",
    "running",
    "zombie"
  };

  int count = getprocs(procs, NPROC);

  if(count < 0){
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }

  printf("PID\tPPID\tSTATE\tSIZE\tNAME\n");

  for(int i = 0; i < count; i++){
    int state = procs[i].state;
    char *state_name = "unknown";

    if(state >= 0 && state < 6)
      state_name = states[state];

    printf("%d\t%d\t%s\t%lu\t%s\n",
           procs[i].pid,
           procs[i].ppid,
           state_name,
           procs[i].sz,
           procs[i].name);
  }

  exit(0);
}
