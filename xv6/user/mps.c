#include "pstat.h"
//#include "defs.h"
#include "types.h"
//#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  struct pstat m_ps;
  getpinfo(&m_ps);
  int index;
  for (index=0; index< NPROC; index++){
    printf(1,"inuse: %d,",m_ps.inuse[index]);
    printf(1,"pid: %d,",m_ps.pid[index]);
    printf(1,"high tick: %d,",m_ps.hticks[index]);
    printf(1,"low tick: %d,\n",m_ps.lticks[index]);
  }
  /* printf(1,"after setting tickets\n\n"); */
  /* settickets(3); */
  /* getpinfo(&m_ps); */
  /* for (index=0; index< NPROC; index++){ */
  /*   printf(1,"inuse: %d,",m_ps.inuse[index]); */
  /*   printf(1,"pid: %d,",m_ps.pid[index]); */
  /*   printf(1,"high tick: %d,",m_ps.hticks[index]); */
  /*   printf(1,"low tick: %d,\n",m_ps.lticks[index]); */
  /* } */
  exit();
}
