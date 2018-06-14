/*
 * test4.cpp - Block thread. spawns a thread and block it after two rounds.
 *
 * Output should be:
test4:
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
int maxRounds = 5;
int n = 1;

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

void fsus()
{
	int id = uthread_get_tid();
	int rounds = 0;
	while (rounds < maxRounds){
		if(currId != id){
            currId = id;
			printf("***%d***\n", id);
			if (rounds == 2){
				uthread_block(1);
			}
			rounds++;
			fflush(stdout);
			
		}
	}
	uthread_terminate(id);
}


int main(int argc, char **argv)
{
    printf("test4:\n--------------\n");
    int tid;

    uthread_init(quantumR);

    tid = uthread_spawn(f);
 	uthread_spawn(f);

    
    if (tid == -1)
    	fprintf(stderr, "unjustified failrure to spawn\n");
    fflush(stdout);
    fsus();

    uthread_terminate(0);
    return 0;
}
