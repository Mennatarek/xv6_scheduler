/* test cv_wait and cv_signal, cannot leave cv_wait without lock */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 0;
lock_t lock;
cond_t cond;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   lock_init(&lock);

   printf(1, "before creating thread \n");
   int thread_pid = thread_create(worker, 0);
   assert(thread_pid > 0);
   printf(1, "created thread,parent sleep waiting for lock \n");   

   sleep(20);
   printf(1, "in parent thread, after sleep(20), try to grab lock \n");
   printf(1, "lock value:%d \n",lock);                
   lock_acquire(&lock);
   printf(1, "in parent thread, grabbed lock \n");          
   global = 2;
   cv_signal(&cond);
   sleep(50);
   global = 1;
   lock_release(&lock);

   int join_pid = thread_join();
   assert(join_pid == thread_pid);

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
  printf(1, "in child thread, waiting for lock \n");     
  lock_acquire(&lock);
  printf(1, "in child thread, grabbed lock \n");       
  assert(global == 0);
  cv_wait(&cond, &lock);
  assert(global == 1);
  lock_release(&lock);
  exit();
}

