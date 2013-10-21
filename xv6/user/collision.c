#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int argc, char* argv[]){
  int num = 161792; // should need 8K stack size
  int largeArray[num];
  printf(1,"large array at addr:%x \n", &largeArray);
  char *p;
  p=sbrk(100);
  printf(1,"p is %x \n",p);
  exit();
  return 0;
}

