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
  np->stack = stack;
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

int
join(void **stack)
{
  struct proc *p;
  int havekids, pid;

  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for zombie children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != proc || p->pgdir != proc->pgdir)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        *stack = p->stack; // Assuming we store stack in struct proc for join
        kfree(p->kstack);
        p->kstack = 0;
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || proc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(proc, &ptable.lock);  //DOC: wait-sleep
  }
}
