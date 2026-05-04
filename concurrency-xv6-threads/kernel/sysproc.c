#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

int
sys_sbrk(void)
{
  int n;
  int addr;

  if(argint(0, &n) < 0)
    return -1;
  
  // We need to protect the growth of the address space with a lock
  // because multiple threads could be calling sbrk() simultaneously.
  acquire(&proc->parent->lock); // Assuming a shared lock in the process group
  addr = proc->sz;
  if(growproc(n) < 0) {
    release(&proc->parent->lock);
    return -1;
  }
  release(&proc->parent->lock);
  return addr;
}
