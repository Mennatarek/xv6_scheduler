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

