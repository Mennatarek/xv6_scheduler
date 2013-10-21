#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  
  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpu->id == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpu->id, tf->cs, tf->eip);
    lapiceoi();
    break;
   
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpu->id, tf->eip, rcr2());
      panic("trap");
    }

    //////////////////////p3.2////////////////////////////
    // when stack needs to grow (size of current stack > allocated stack)
    // -PGSIZE to eliminate the size of invalid page between heap and stack
    //    cprintf("in kernel/trap.c: tf->esp %d\n",tf->esp);
    int stack_sz=proc->stack_sz;
    cprintf("stack_sz:%d\n",stack_sz);
    //    cprintf("stack pointer:%d\n",tf->esp);
    //get the virtual addr that causes the trap
    int invalidAddr=rcr2();
    cprintf("invalidAddr:%d\n",invalidAddr);
    // have some doubts when tf->esp == (USERTOP - ....)
    // may be the situation when the user is allocating some stuff, but the stack pointer
    // hasn't changed
    // compiler do wierd things about stack pointer. It will use space above sp
    // require the virtual addr that is causing problem to be within 1 page of the stack
    if ( (T_PGFLT == tf->trapno) &&  (invalidAddr >= (USERTOP - (proc->stack_sz - PGSIZE) - PGSIZE)) &&   (invalidAddr < (USERTOP - (proc->stack_sz - PGSIZE))) ){
      //check if we are going to overwrite on heap
      if ( (USERTOP - proc->stack_sz - proc->sz) >= PGSIZE ){
        //when there is still one or more pgsize left
        // get the allocated size for stack
        int ori_stack_sz=proc->stack_sz-PGSIZE;
        // get the actual size intended to grow
        // only grow 1 page at a time
        int grow_sz=PGSIZE;
        int tmp;
        // allocate another page for stack
        if ((tmp=allocuvm(proc->pgdir,
                          (USERTOP-ori_stack_sz-grow_sz),
                          USERTOP-ori_stack_sz ))!=0) {//when success
          proc->stack_sz = proc->stack_sz+grow_sz;
          cprintf("valid stack page top:%d\n",USERTOP - (proc->stack_sz - PGSIZE));
        return;
        }
      }
    }
    ///////////////////////////////////////////////////////////////////
    
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpu->id, tf->eip, 
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running 
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}
