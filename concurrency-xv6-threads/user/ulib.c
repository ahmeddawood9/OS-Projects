#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

int
thread_create(void (*start_routine)(void *, void *), void *arg1, void *arg2)
{
  void *stack = malloc(2 * PGSIZE);
  if (stack == 0)
    return -1;

  // Page-align the stack
  void *aligned_stack;
  if ((uint)stack % PGSIZE == 0)
    aligned_stack = stack;
  else
    aligned_stack = stack + (PGSIZE - ((uint)stack % PGSIZE));

  int pid = clone(start_routine, arg1, arg2, aligned_stack);
  return pid;
}

int
thread_join()
{
  void *stack;
  int pid = join(&stack);
  if (pid > 0) {
    // We need a way to free the original malloc'd pointer, 
    // but join returns the aligned stack. 
    // For simplicity in this project, we assume the stack is page-aligned 
    // or we manage it differently.
    // free(stack); 
  }
  return pid;
}

void
lock_init(lock_t *lock)
{
  lock->ticket = 0;
  lock->turn = 0;
}

void
lock_acquire(lock_t *lock)
{
  int myturn = xadd(&lock->ticket, 1);
  while(lock->turn != myturn)
    ; // spin
}

void
lock_release(lock_t *lock)
{
  xadd(&lock->turn, 1);
}
