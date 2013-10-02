#include "pstat.h"
//#include "../kernel/defs.h"
#include "types.h"
//#include "stat.h"
#include "user.h"

void printPS(struct pstat * m_ps){
  getpinfo(m_ps);
  int index;
  for (index=0; index<10; index++){
    printf(1,"inuse: %d,",m_ps->inuse[index]);
    printf(1,"pid: %d,",m_ps->pid[index]);
    printf(1,"high tick: %d,",m_ps->hticks[index]);
    printf(1,"low tick: %d,\n",m_ps->lticks[index]);
  }
}

void spin(int num){
  int a=0;
  int x=0;
  for (a=0; a < num * 1000;a++){
    x+=a;
  }
}

int
main(int argc, char *argv[])
{
  struct pstat *m_ps;
  m_ps = malloc(sizeof(struct pstat));
  spin(133);
  //  getpinfo(m_ps);
  /* int index; */
  /* for (index=0; index<10; index++){ */
  /*   printf(1,"inuse: %d,",m_ps->inuse[index]); */
  /*   printf(1,"pid: %d,",m_ps->pid[index]); */
  /*   printf(1,"high tick: %d,",m_ps->hticks[index]); */
  /*   printf(1,"low tick: %d,\n",m_ps->lticks[index]); */
  /* } */
  printPS(m_ps);
  settickets(1000);
  spin(133);
  printf(1,"issue the second getpinfo \n");
  printPS(m_ps);
  /* getpinfo(m_ps); */
  /* for (index=0; index<10; index++){ */
  /*   printf(1,"inuse: %d,",m_ps->inuse[index]); */
  /*   printf(1,"pid: %d,",m_ps->pid[index]); */
  /*   printf(1,"high tick: %d,",m_ps->hticks[index]); */
  /*   printf(1,"low tick: %d,\n",m_ps->lticks[index]); */
  /* } */
  int rc=fork();
  if (0 == rc){
    settickets(1);
    write(1,"child\n",sizeof("child\n"));
    //child process
    spin(30000);
    //    settickets(10);
    //    spin(3000);
    exit();
  } else if (0<rc){
    write(1,"parent\n",sizeof("parent\n"));
    //parent
    spin(30000);
    wait();
    getpinfo(m_ps);
    int index;
    for (index=0; index<10; index++){
      printf(1,"inuse: %d,",m_ps->inuse[index]);
      printf(1,"pid: %d,",m_ps->pid[index]);
      printf(1,"high tick: %d,",m_ps->hticks[index]);
      printf(1,"low tick: %d,\n",m_ps->lticks[index]);
    }


    exit();
  }
  exit();
  /* struct pstat m_ps; */
  /* getpinfo(&m_ps); */
  /* int index; */
  /* for (index=0; index< NPROC; index++){ */
  /*   printf(1,"inuse: %d,",m_ps.inuse[index]); */
  /*   printf(1,"pid: %d,",m_ps.pid[index]); */
  /*   printf(1,"high tick: %d,",m_ps.hticks[index]); */
  /*   printf(1,"low tick: %d,\n",m_ps.lticks[index]); */
  /* } */
  /* printPS(); */
  /* settickets(10); */
  /* printf(1,"after setting tickets\n\n"); */
  /* int rc=fork(); */
  /* if (0 == rc){ */
  /*   write(1,"child\n",sizeof("child\n")); */
  /*   //child process */
  /*   spin(3000); */
  /*   settickets(10); */
  /*   spin(3000); */
  /*   exit(); */
  /* } else if (0<rc){ */
  /*   write(1,"parent\n",sizeof("parent\n")); */
  /*   //parent */
  /*   printPS(); */
  /*   spin(3000); */
  /*   printPS(); */
  /*   spin(3000); */
  /*   printPS(); */
  /*   wait(); */
  /* } */

  
  /* exit(); */
  /* printf(1,"set tickets to 1\n\n"); */
  /* spin(atoi(argv[1])); */
  /* printPS(); */
  /* printf(1,"2nd print ps\n"); */
  /* printPS(); */
  /* settickets(atoi(argv[2])); */
  /* printf(1,"set tickets to %d\n\n",atoi(argv[2])); */
  /* spin(atoi(argv[1])); */
  /* printPS(); */
  /* exit(); */
}
