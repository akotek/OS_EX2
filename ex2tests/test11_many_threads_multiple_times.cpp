/*
 * test11.cc - Create many threads: Create as many threads as allowed, then terminate all of them,
 * then create again many threads, and so forth for several times - check that nothing crashes.
 *
 * Output should be:
test11:
--------------
 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <map>
#include <list>
#include "../uthreads.h"


int quantumR = 5000;
int currId = -1;
int maxRounds = 1;

/* Just prints its ID after process switch. */
void f()
{
    while(1){}
}

void testMaxNum() {
  printf("test11:\n--------------\n");
  
  for(int j = 0; j < 4; j++) {
    for(int i = 0; i < MAX_THREAD_NUM-1; ++i) {
      uthread_spawn(f);
    }
    for(int i = 0; i < MAX_THREAD_NUM-1; ++i) {
      uthread_terminate(i+MAX_THREAD_NUM*j);
    }
  }
  
}

int main(int argc, char* argv[])
{
  uthread_init(quantumR);
  testMaxNum();
  uthread_terminate(0);
}
