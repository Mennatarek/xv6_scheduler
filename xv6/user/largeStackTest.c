#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int argc, char* argv[]){
  //  int num = 620 * 1024/4; // should need 8K stack size
  int num = 632 * 1024/4; // should need 8K stack size
  int largeArray[num];
  printf(1,"size of an int:%d \n", sizeof(int));
  printf(1,"size of the large array:%d \n", sizeof(largeArray));  
  printf(1,"large array at addr:%x \n", &largeArray);
  char *p=sbrk(100);
  printf(1,"try allocating a large heap. p :%x \n", p);  
  exit();
  return 0;
}
