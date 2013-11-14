#include "types.h"
//#include "stat.h"
//#include "fcntl.h"
#include "user.h"
#include "x86.h"

//my own thread library: optimization:figure out where the separate file should go if separated
//create a new thread
#define PGSIZE (4096)
int thread_create(void (*start_routine)(void*), void *arg){
  void *stack = malloc((uint)PGSIZE*2);
  if (NULL==stack){ //failed to malloc
    printf(1,"malloc failed in thread_create\n");
    return -1;
  }
  if((uint)stack % PGSIZE) // the stack must be page aligned
    stack = stack + (4096 - (uint)stack % PGSIZE);
  int clone_pid = clone(start_routine, arg, stack);
  return clone_pid;
}

//join the thread
int thread_join(){
  void *join_stack;
  int join_pid = join(&join_stack);
  free(join_stack);
  return join_pid;
}

//all about lock
void lock_init(lock_t * mlock){
  mlock->locked=0;
  return;
}

void lock_acquire(lock_t * mlock){
  // The xchg is atomic.
  // It also serializes, so that reads after acquire are not
  // reordered before it. 
  while(xchg(&mlock->locked, 1) != 0) //if held
    ;
  return;
}

void lock_release(lock_t * mlock){
  xchg(&mlock->locked, 0);
  return;
}

//all about conditional variables
void cv_wait(cond_t * mcond, lock_t * outsidelock){
  //ousidelock is acquired right now
  //sleep on mcond
  //release mlock
  //the whole operation is atomic
  printf(1,"in cv_wait, try to grab cv lock: \n");
  printf(1,"in cv_wait, initial cv lock value: %d\n", mcond->mlock->locked);  
  lock_acquire(mcond->mlock);
  printf(1,"in cv_wait, got cv lock: %d\n");  
  //add self to list
  int curpid=getpid();
  mcond->waitingList[mcond->idx] = curpid;
  (mcond->idx)++;
  printf(1,"add pid to waiting: %d, next available idx: %d\n",curpid,mcond->idx);
  lock_release(outsidelock);  
  lock_release(mcond->mlock);
  printf(1,"in cv_wait, release outside and cv lock: %d\n");    
  //bug, what if scheduler interrupt here???
  //put cur thread to sleep
  /* threadSleep(); */
  sleep(100);
  lock_acquire(outsidelock);  
}

void cv_signal(cond_t * mcond){
  //release mlock
  //the whole operation is atomic
  lock_acquire(mcond->mlock);
  (mcond->idx)--;  
  int curpid=mcond->waitingList[mcond->idx];
  printf(1,"going to wake up pid: %d, at idx: %d\n",curpid, mcond->idx);  
  threadWake(curpid);
  lock_release(mcond->mlock);
}

