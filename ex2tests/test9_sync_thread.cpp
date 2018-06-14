/*
 * test9.cpp - Sync thread. spawns 2 threads, then sync the first with the second
 * Output should be:
test9:
--------------
***0***
***1***
***2***
***0***
***1***
***2***
***0***
***1***
***2***
***0***
***2***
***0***
***2***
***0***
***1***
***0***
***1***
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
int maxRounds = 5;
int maxRounds2 = 8;
int tidToTerminate = 1;
clock_t start;

/* Just prints its ID after process switch. */
void f()
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

/* Just prints its ID after process switch. */
void f2()
{
    int id = uthread_get_tid();
    int rounds = 0;
    while (rounds < maxRounds2){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            rounds++;
            fflush(stdout);
        }
    }
    uthread_terminate(id);
}

/* Just prints its ID after process switc.*/
void fsus()
{
    int id = uthread_get_tid();
    int rounds = 0;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            if (rounds == 2){
            	uthread_sync(2);
            }
            rounds++;    
            fflush(stdout);
      	}
   }
   uthread_terminate(id);
}


int main(int argc, char **argv)
{
    printf("test9:\n--------------\n");
    start = clock();
    uthread_init(quantumR);
    uthread_spawn(fsus);
    uthread_spawn(f);
    f2();
    return 0;
}
