#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"
#include "spinlock.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//return how number of syscall issued (p1.2)
//global declaration in sysfunc.h
int numsyscall=0;
int
sys_getsyscallinfo(void)
{
  return numsyscall;
}

/////////////////p2.2 scheduler ////////////////////////////////
//set the tickets for the particular process (p2.2).
int
sys_settickets(void)
{
  int ticketNum;
  if(argint(0, &ticketNum) < 0)
    return -1;
  else if (1 > ticketNum)
    return -1;
  else{
    //set the tickets number of current process
    proc->tickets = ticketNum;
    return 0;
  }
}

struct ptable_t {
  struct spinlock lock;
  struct proc proc[NPROC];
};

//get the process info
int
sys_getpinfo(void)
{
  struct pstat *ps=NULL;
  if(argpstat(0, ps) < 0)
    return -1;
  else{
    //return the system process information in to pstat
    struct proc *p;
    extern struct ptable_t ptable;
    // Loop over process table looking for information
    acquire(&(ptable.lock));
    int index=0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      ps->pid[index]=p->pid;
      if (p->state == UNUSED)
        ps->inuse[index]=0;
      else
        ps->inuse[index]=1;
      if (1 == p->priority){//high priority
        ps->hticks[index]= p->tickets;
        ps->lticks[index]=0;
      } else if (0 == p->priority) { // low priority
        ps->lticks[index]= p->tickets;
        ps->hticks[index]=0;
      } else {
        panic("sys_getpinfo!!!: the priority of the process is not defined");
      }
      index++;
    }
    release(&ptable.lock);
    return 0;
  }
  ////////////////////////////////////////////////////////////
}

