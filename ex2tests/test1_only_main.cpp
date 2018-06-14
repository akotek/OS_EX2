/*
 * test1.cc - 1. A single thread (the main thread). Only initialize the library, see that quantums elapse properly.
 * Check that the quantum (less than a second) matches your input to the library.
 * 1a. Do the same with a quantum larger than one second (expressed in microseconds). Check that it works properly.
 * 
 * Output should be:
 * 
 * test1:
 * --------------
 *
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

void test(int quantumR,const char * testtype) {
	clock_t start = clock();
	uthread_init(quantumR);
	for(int i = 0; i < 20; ++i) {
		for(volatile int j = 0; j < 50000000; ++j)
		{}
	}
	clock_t time = clock() - start;
	float expectedNumQuantums = (float)time/quantumR;
	int actualNumOfQuantums = uthread_get_quantums(0);
	int totalNumOfQuantums = uthread_get_total_quantums();
	if (expectedNumQuantums*2 < actualNumOfQuantums || expectedNumQuantums/2 > actualNumOfQuantums) {
		printf("FAIL %s: quantums = %d should be = %f\n",testtype,actualNumOfQuantums,expectedNumQuantums);
	}
	if (expectedNumQuantums*2 < totalNumOfQuantums || expectedNumQuantums/2 > totalNumOfQuantums) {
		printf("FAIL %s: total quantums (%d) != expected quantums (%f)\n",testtype,totalNumOfQuantums,expectedNumQuantums);
	}
}

int main(int argc, char* argv[])
{
	printf("test1:\n--------------\n");
	test(10000,"small quantum");
}
