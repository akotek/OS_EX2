/*
 * test8.cc - Terminating the main thread: Create a thread, who then terminates the main thread. Program should exit nicely.
 * Output should be:
test8:
--------------
***0***
***1***
***0***
***1***
Program should terminate after this line
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
int tidToTerminate = 0;

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
	    printf("Program should terminate after this line\n");
	    uthread_terminate(tidToTerminate);
	    terminateFlag = 0;
        }

    }
    uthread_terminate(id);


}


int main(int argc, char **argv)
{
    printf("test8:\n--------------\n");
    uthread_init(quantumR);
    tidToTerminate = 0;
    uthread_spawn(fsus);
    fflush(stdout);
    f();
    return 0;
}
