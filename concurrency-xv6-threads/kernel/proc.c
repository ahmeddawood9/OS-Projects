#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

// ... existing code ...

int
clone(void(*fcn)(void *, void *), void *arg1, void *arg2, void *stack)
{
  int i, pid;
  struct proc *np;
  uint sz, sp;

  // Allocate process.
  if((np = allocproc()) == 0)
    return -1;

  // Share address space.
  np->pgdir = proc->pgdir;
  np->sz = proc->sz;
  np->parent = proc;
  *np->tf = *proc->tf;

  // Clear %eax so that clone returns 0 in the child.
  np->tf->eax = 0;

  // Set up user stack.
  // The stack should be page-aligned and one page in size.
  // We need to push the arguments and a fake return address onto it.
  sp = (uint)stack + PGSIZE;
  
  sp -= 4;
  *(uint*)sp = (uint)arg2;
  sp -= 4;
  *(uint*)sp = (uint)arg1;
  sp -= 4;
  *(uint*)sp = 0xffffffff; // Fake return address

  np->tf->esp = sp;
  np->tf->ebp = sp;
  np->tf->eip = (uint)fcn;

  // Copy file descriptors.
  for(i = 0; i < NOFILE; i++)
    if(proc->ofile[i])
      np->ofile[i] = filedup(proc->ofile[i]);
  np->cwd = idup(proc->cwd);

  safestrcpy(np->name, proc->name, sizeof(proc->name));

  pid = np->pid;

  acquire(&ptable.lock);
  np->state = RUNNABLE;
  release(&ptable.lock);

  return pid;
}
