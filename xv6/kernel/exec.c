#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;

  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  //try to setup the kernel part of a page table
  if((pgdir = setupkvm()) == 0)
    goto bad;
  
  //  cprintf("page directory addr when first assigned: %x\n",pgdir);

  //////////////////////////////////////p3.1//////////////
  //call method in vm.c to make an invalid page entry
  //for address 0 with the length of 1 page size
  //start the valid virtual address from 1 page size
  sz=PGSIZE;
  /////////////////////////////////////////////////////////
  
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;

    //ph.va is 0x1000 when user makefile has changed
    //    cprintf("ph.va: %d\n",ph.va);
    
    if((sz = allocuvm(pgdir, sz, ph.va + ph.memsz)) == 0)
      goto bad;

    //load the bytes from file into memory
    if(loaduvm(pgdir, (char*)ph.va, ip, ph.offset, ph.filesz) < 0)
      goto bad;

  }
  iunlockput(ip);
  ip = 0;

  // Allocate a one-page stack at the next page boundary
  /////////////////p3.2/////////////////
  //still need to round up, since proc->sz get sz
  sz = PGROUNDUP(sz);
  /* if((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0) */
  /*   goto bad; */
  /* sp = sz; */

  /////////////////////////p3.2/////////////////////////
  // allocate the last page in user space for stack
  int stack_top; //top of stack. (end of the virtual address,since stack grows backward)
  if((stack_top = allocuvm(pgdir, USERTOP-PGSIZE, USERTOP)) == 0)
    goto bad;
  ///////////////////////////////////////////////////

  //cprintf("stack end addr: %d\n",USERTOP-PGSIZE);
  // Push argument strings, prepare rest of stack in ustack.
  sp = stack_top;

  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp -= strlen(argv[argc]) + 1;
    sp &= ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;
  
  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldpgdir = proc->pgdir;
  proc->pgdir = pgdir;
  proc->sz = sz;
  ////////////////p3.2//////////////////////
  proc->stack_sz=PGSIZE+PGSIZE; // note only 1 PG is allocated right now.
  // add another PGSIZE to make sure there is always 1 invalid page
  // between heap and stack
  //////////////////////////////////////////
  
  //when first called, start init process in the user program
  //  cprintf("in kernel: elf.entry: %d\n",elf.entry);
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;


  switchuvm(proc);
  freevm(oldpgdir);

  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip)
    iunlockput(ip);
  return -1;
}
