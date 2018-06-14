/*
 * test7.cc - Self termination: Create 2 threads and let the first thread terminate itself. Check that the main and second thread continue as expected.
 *
 * Output should be: (self terminating quantum should be <1)
test7:
--------------
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
int maxRounds = 4;
int tidToTerminate = 1;

/* Just prints its ID after process switch. */
void f()
{
    int id = uthread_get_tid();
    int rounds = -1;
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

/* Just prints its ID after process switch and suspend the given tid. (itself)*/
void fsus()
{
    int id = uthread_get_tid();
    int rounds = -1;
    int terminateFlag = 1;
    while (rounds < maxRounds){
        if(currId != id){
            currId = id;
            printf("***%d***\n", id);
            rounds++;
            fflush(stdout);
        }
        if (rounds == 1 && terminateFlag) {
	    	uthread_terminate(tidToTerminate);
	    	terminateFlag = 0;
        }
    }
    uthread_terminate(id);


}


int main(int argc, char **argv)
{
    printf("test7:\n--------------\n");
    uthread_init(quantumR);
    tidToTerminate = uthread_spawn(fsus);
    uthread_spawn(f);
    fflush(stdout);
    f();
    uthread_terminate(0);
    return 0;
}
