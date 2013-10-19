#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int argc, char* argv[]){
  int * test = 0;
  printf(1,"try to dereference a null pointer: %d\n",*test);
  exit();
  return 0;
}
