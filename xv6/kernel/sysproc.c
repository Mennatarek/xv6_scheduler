#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

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

// grow user memory space
//growproc is in proc.c
// seems to return the old memory space size
// the paramter passed can be positive or negative
// when negative -- shrink user memory space
int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  ////////////////////p3.2///////////////////////
  // add check: prevent heap overwritting on stack
  ///////////////////////////////////////////////
  // stack_sz will always keep as the multiples of PGSIZE  
  int stack=proc->stack_sz;
  int page_n = PGROUNDUP(n);
  int heap = PGROUNDUP(proc->sz);
  if (heap + page_n > USERTOP - stack){
    panic("from sys_sbrk: heap is going to overwriting stack!");
    return -1;
  }
  ///////////////////////////////////////////////////////
    
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
