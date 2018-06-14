/*
 * test12.cpp - resume synced and blocked. spawn 2 threads, then sync the first to the second, then block the first, then resume the first, see that it doesn't run again until the second terminates
 * Output should be:
test12:
--------------
***0***
***1***
***2***
***0***
***2***
***0***
***2***
***0***
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
int maxRounds = 6;
int maxRounds2 = 10;
int tidToTerminate = 1;
clock_t start;

/* Just prints its ID after process switch. */
void f2()
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

void f1()
{
    int id = uthread_get_tid();
    int rounds = 0;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            if (rounds == 0){
            	uthread_sync(2);
            }
            rounds++;
            fflush(stdout);
        }
    }
    uthread_terminate(id);


}

/* Just prints its ID after process switch.*/
void fmain()
{
    int id = uthread_get_tid();
    int rounds = 0;
    int terminateFlag = 1;
    while (rounds < maxRounds2){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            if (rounds == 2){
            	uthread_block(1);
            }
            if (rounds == 4){
            	uthread_resume(1);
            }
            rounds++;    
            fflush(stdout);
      	}
   	}
   	uthread_terminate(id);
	
}


int main(int argc, char **argv)
{
    printf("test12:\n--------------\n");
    start = clock();
    uthread_init(quantumR);
    uthread_spawn(f1);
    uthread_spawn(f2);
    fflush(stdout);
    fmain();
    return 0;
}
