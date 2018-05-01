#include <cstdio>
#include <csetjmp>
#include <csignal>
#include <cassert>
#include <vector>
#include <sys/time.h>
#include <iostream>
#include "uthreads.h"
using namespace std;

//TODO: -delete asserts after
//TODO: change CERR to printing in all func's
//TODO: make validation input static func's and remove from all functions

// Constants:
// ----------------
static enum State {READY, WAITING, BLOCKED};
#define JB_SP 6
#define JB_PC 7
typedef unsigned long address_t;
// ----------------

// data structures:
// ----------------
static struct Thread {
    char stack[STACK_SIZE];
    sigjmp_buf env{};
    int id;
    State state;

    Thread(int id, State state){
        this->id = id;
        this->state = state;
    }

};
// ----------------

// global/static variables
// ----------------
static int globalThreadCounter = 0;
static int totalSizeOfQuantums = 0;
static vector <Thread> allThreadList;
static vector <Thread*> readyThreadPtrList; // represents readyQueue
static vector <Thread*> blockedThreadPtrList; // represents blockedListOfThreads
// ----------------

// helper functions:
// ----------------
namespace helperFuncs{
    address_t translate_address(address_t addr)
    {
        address_t ret;
        asm volatile("xor    %%fs:0x30,%0\n"
                "rol    $0x11,%0\n"
        : "=g" (ret)
        : "0" (addr));
        return ret;
    }

    // add another helper funcs here
}
// ----------------

Thread initThread(void (*f)(void), int threadId=globalThreadCounter){
    if (f == nullptr) return Thread{0, READY}; //init mainThread
    address_t sp, pc;

    Thread newThread (threadId, READY);
    sp = (address_t)newThread.stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)f; // sets thread to work on func f

    sigsetjmp(newThread.env, 1);
    (newThread.env->__jmpbuf)[JB_SP] = helperFuncs::translate_address(sp);
    (newThread.env->__jmpbuf)[JB_PC] = helperFuncs::translate_address(pc);
    sigemptyset(&newThread.env->__saved_mask);

    return newThread;
}

void roundRobinAlgorithm(int sig){
    cerr << "Wrong quantom input" << endl;
}

int setTimerSignalHandler(int quantomUsecs){
    struct sigaction sa;
    struct itimerval timer;

    // Install RR algorithm as the signal handler for SIGVTALRM:
    sa.sa_handler = &roundRobinAlgorithm;
    if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
        printf("sigaction error.");
    }

    // Start counting time:
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 3;
    timer.it_interval.tv_usec = 0;
    if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
        cerr << "set_timer error" << endl;
        return -1;
    }
}
/*
 * Description: This function initializes the Thread library.
 * You may assume that this function is called before any other Thread library
 * function, and that it is called exactly once.
*/
int uthread_init(int quantum_usecs){
    if (quantum_usecs < 0)
    {
        cerr << "Wrong quantom input" << endl;
        return -1;
    }
    int mainThread = uthread_spawn(nullptr); // init mainThread
    totalSizeOfQuantums++;
    assert(mainThread != -1);

    int setTimer = setTimerSignalHandler(quantum_usecs);
    assert(setTimer != -1);

    return 0;
}

/*
 * Description: This function creates a new Thread, whose entry point is the
 * function f with the signature void f(void). The Thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each Thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created Thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void)){
//    assert(f != nullptr);
    if (allThreadList.size() > MAX_THREAD_NUM)    {
        cerr << "Num of threads exceeded" << endl;
        return -1;
    }
    struct Thread thread1 = initThread(f);
    allThreadList.push_back(thread1);
    globalThreadCounter +=1;

    Thread* thread1Ptr = &(allThreadList[allThreadList.size()-1]);
    readyThreadPtrList.push_back(thread1Ptr);

    return thread1.id;
}

int findThreadById(int threadId)
{
    assert(threadId >= 0);
    for(vector<Thread>::size_type i = 0; i != allThreadList.size(); i++) {
        /* std::cout << someVector[i]; ... */
        if (allThreadList[i].id == threadId) return (int)i;
    }
    cerr << "Thread not found in Thread main vector" << endl;
    return -1;
}
int findThreadPtrById(int threadId){
    return 1; //TODO
}
/*
 * Description: This function terminates the Thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this Thread should be released. If no Thread with ID tid
 * exists it is considered as an error. Terminating the main Thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the Thread was successfully
 * terminated and -1 otherwise. If a Thread terminates itself or the main
 * Thread is terminated, the function does not return.
*/
int uthread_terminate(int tid){
    if (tid < 0)
    {
        cerr << "thread library error::wrong id input in uterminate" << endl;
        return -1;
    }

    if (tid == 0){
        cerr << "thread library error::terminating main thread, exiting" <<
                                                                         endl;
        exit(0);
    }

    int threadIndex = findThreadById(tid);
    if (threadIndex == -1){
        cerr << "thread library error::given thread does not exist" << endl;
        return -1;
    }

    if (allThreadList[threadIndex].state == READY);
    allThreadList.erase(allThreadList.begin() + threadIndex);
}


/*
 * Description: This function blocks the Thread with ID tid. The Thread may
 * be resumed later using uthread_resume. If no Thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main Thread (tid == 0). If a Thread blocks itself, a scheduling decision
 * should be made. Blocking a Thread in BLOCKED state has no
 * effect and is not considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid);


/*
 * Description: This function resumes a blocked Thread with ID tid and moves
 * it to the READY state. Resuming a Thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no Thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid);


/*
 * Description: This function blocks the RUNNING Thread until Thread with
 * ID tid will move to RUNNING state (i.e.right after the next time that
 * Thread tid will stop running, the calling Thread will be resumed
 * automatically). If Thread with ID tid will be terminated before RUNNING
 * again, the calling Thread should move to READY state right after Thread
 * tid is terminated (i.e. it won’t be blocked forever). It is considered
 * as an error if no Thread with ID tid exists or if the main Thread (tid==0)
 * calls this function. Immediately after the RUNNING Thread transitions to
 * the BLOCKED state a scheduling decision should be made.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid);


/*
 * Description: This function returns the Thread ID of the calling Thread.
 * Return value: The ID of the calling Thread.
*/
int uthread_get_tid();


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums();


/*
 * Description: This function returns the number of quantums the Thread with
 * ID tid was in RUNNING state. On the first time a Thread runs, the function
 * should return 1. Every additional quantum that the Thread starts should
 * increase this value by 1 (so if the Thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * Thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the Thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid);

