#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int argc, char* argv[]){
  int num = 2000; // should need 8K stack size
  int largeArray[num];
  printf(1,"large array at addr:%x \n", &largeArray);
  exit();
  return 0;
}
