#include "types.h"
#include "stat.h"
#include "user.h"

lock_t lock;
int counter = 0;

void
worker(void *arg1, void *arg2)
{
  int n = (int)arg1;
  for (int i = 0; i < n; i++) {
    lock_acquire(&lock);
    counter++;
    lock_release(&lock);
  }
  exit();
}

int
main(int argc, char *argv[])
{
  int n_threads = 5;
  int increments = 1000;
  
  lock_init(&lock);
  
  printf(1, "Starting %d threads, each incrementing %d times...\n", n_threads, increments);
  
  for (int i = 0; i < n_threads; i++) {
    thread_create(worker, (void*)increments, 0);
  }
  
  for (int i = 0; i < n_threads; i++) {
    thread_join();
  }
  
  printf(1, "Final counter value: %d (expected %d)\n", counter, n_threads * increments);
  
  exit();
}
