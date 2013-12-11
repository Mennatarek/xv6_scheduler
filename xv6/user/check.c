#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
  int fd=open("foo", O_CREATE|O_CHECKED|O_RDWR);
  printf(1,"fd: %d\n",fd);
  char content[1] ="E"; //hex 44
  int value=write(fd, content,sizeof(content));
  printf(1,"in user program. return value from write: %d\n",value);
  value=close(fd);
  printf(1,"in user program. return value from close: %d\n",value);
  fd=open("foo", O_CREATE|O_CHECKED|O_RDWR);
  value=read(fd, content,sizeof(content));
  printf(1,"in user program. return value from read: %d\n",value);  
  exit();
}
