#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


int main (int argc, char* argv[]){
  char *test = (char *)(640*1024-4096);
  *test='a';
  printf(1,"test end of va: %x\n",test);
  exit();
  return 0;
}
