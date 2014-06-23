#include "fuck.h"

#include <stdio.h>

#include <pthread.h>
#include <sys/syscall.h>

void tid()
{
  //int id = pthread_self();
  //printf("Thread id=%u!\n", id);

  int id2 = syscall(SYS_gettid);
  printf("Thread id=%u -->\n", id2);
}

void fuck()
{
  printf("Fuck!\n");
}
