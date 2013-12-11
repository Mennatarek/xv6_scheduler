// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

#define N  3

void
clonePrint(void * cloneNum);


void
clonetest(void)
{
  int n, pid;
  void (*fcn)(void *);
  fcn=&clonePrint;
  printf(1,"size of void %d\n",sizeof(void));
  printf(1,"size of void* %d\n",sizeof(void*));  
  printf(1,"size of int %d\n",sizeof(int));
  printf(1,"size of char %d\n",sizeof(char));  
  //  char i='a';
  //  (*fcn)((void*)&i);

  printf(1, "clone test\n");
  printf(1, "function addr: %p\n", (void *)fcn);
  printf(1, "arg addr:%x\n",&n);

  for(n=0; n<N; n++){
    printf(1, "n's value:%d\n",n);
    void * mStack = (void*)malloc(4096);
    printf(1, "stack addr:%x\n",mStack);    
    pid = clone(fcn,(void *)&n,mStack);
    if(pid < 0)
      break;
    if(pid == 0){
      exit();
    }
    if(pid>0)
      join(&mStack);  
  }

  /* if(n == N){ */
  /*   printf(1, "fork claimed to work N times!\n", N); */
  /*   exit(); */
  /* } */
  
  /* for(; n > 0; n--){ */
  /*   if(wait() < 0){ */
  /*     printf(1, "wait stopped early\n"); */
  /*     exit(); */
  /*   } */
  /* } */
  
  /* if(wait() != -1){ */
  /*   printf(1, "wait got too many\n"); */
  /*   exit(); */
  /* } */
  
  /* printf(1, "fork test OK\n"); */
}

int
main(void)
{
  clonetest();
  exit();
}

void
clonePrint(void * cloneNum)
{
  int* out=(int*)cloneNum;
  printf(1, "clone print called. cloneNum: %d\n",*out);
  exit();
}
