/*
 * test10.cpp - Terminate synced to. spawns 3 threads, then syncs the second to the first, then terminates the second, then terminates the first
 * Output should be:
test10:
--------------
***0***
***1***
***2***
***3***
***0***
***1***
***2***
***3***
***0***
***1***
***2***
***3***
***0***
***1***
***3***
***0***
***1***
***3***
***0***
***1***
***3***
***0***
***1***
***3***
***0***
***3***
***0***
***3***
***0***
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


int quantumR = 1000;
int currId = -1;
int maxRounds = 10;
int tidToTerminate = 1;
clock_t start;

/* Just prints its ID after process switch. */
void f1()
{
    int id = uthread_get_tid();
    int rounds = 0;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            rounds++;
            fflush(stdout);
        }
    }
    uthread_terminate(id);
}

void f2()
{
    int id = uthread_get_tid();
    int rounds = 0;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            if (rounds == 2){
            	uthread_sync(1);
            }
            rounds++;
            fflush(stdout);
        }
    }
    uthread_terminate(id);
}


/* Just prints its ID after process switc.*/
void fmain()
{
    int id = uthread_get_tid();
    int rounds = 0;
    int terminateFlag = 1;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            if (rounds == 5){
            	uthread_terminate(2);
            }
            if (rounds == 7){
            	uthread_terminate(1);
            }
            rounds++;    
            fflush(stdout);
      	}
   	}
   	uthread_terminate(id);
}


int main(int argc, char **argv)
{
    printf("test10:\n--------------\n");
    start = clock();
    uthread_init(quantumR);
    uthread_spawn(f1);
    uthread_spawn(f2);
    uthread_spawn(f1);
    fflush(stdout);
    fmain();
    return 0;
}
