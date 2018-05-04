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

// Constants and DEFINES:
// ----------------
enum State {RUNNING, READY, BLOCKED};
typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7
#define USEC_TO_MICRO  1000000
#define SYS_ERR_MSG "system error: "
#define THREAD_LIB_ERR_MSG "thread library error: "
#define SIGEMPTYSET_ERR_MSG "sigemptyset error."
#define SIGADDSET_ERR_MSG "sigaddset error."
#define SIGPROCMASK_ERR_MSG "sigprocmask error."
#define INVALID_QUANTOM_MSG "invalid quantum size"
#define SIGACTION_ERR_MSG "sigaction error."
#define TIMER_ERR_MSG "setitimer error."
#define MAX_THREAD_ERR_MSG "too many threads, can't create new thread."
#define BAD_TID_ERR_MSG "invalid tid."
#define TERMINATING_MAIN_THREAD_MSG "terminating main thread, existing"
#define THREAD_NOT_FOUND_MSG "thread not found in thread list"
#define MAIN_THREAD_BLOCK_ERR_MSG "error trying to block main thread"
#define SYNC_RUNNING_THRD_MSG "not allowed to sync running thread"
// ----------------

// data structures:
// ----------------
struct Thread {
    int id;
    char stack[STACK_SIZE];
    sigjmp_buf env{};
    State state;
    int quantomsRanByThread;

    Thread(int id){
        this->id = id;
        this->state = READY;
        this->quantomsRanByThread = 0;
    }

};
// ----------------

// global/static variables
// ----------------
static int globalThreadCounter = 0;
static int totalSizeOfQuantums = 0;
struct sigaction sa;
struct itimerval timer;
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
    address_t sp, pc;

    Thread newThread (threadId);
    sp = (address_t)newThread.stack + STACK_SIZE - sizeof(address_t);
    pc = (f != nullptr ? (address_t)f : 0); // 0 if mainThread

    sigsetjmp(newThread.env, 1);
    (newThread.env->__jmpbuf)[JB_SP] = helperFuncs::translate_address(sp);
    (newThread.env->__jmpbuf)[JB_PC] = helperFuncs::translate_address(pc);
    sigemptyset(&newThread.env->__saved_mask);

    return newThread;
}

void setBlockedSignal(){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);
}
void unblockSignal(){
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void roundRobinAlgorithm(int sig){
    cerr << "Wrong quantom input" << endl;
}

int setTimerSignalHandler(int quantomUsecs){

    // Install RR algorithm as the signal handler for SIGVTALRM:
    sa.sa_handler = &roundRobinAlgorithm;
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0) {
        cerr << SIGACTION_ERR_MSG << endl;
    }

    int sec = quantomUsecs / USEC_TO_MICRO;
    int microsec = quantomUsecs % USEC_TO_MICRO;
    // Start counting time:
    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = microsec;
    timer.it_interval.tv_sec = sec;
    timer.it_interval.tv_usec = microsec;
    if (setitimer (ITIMER_VIRTUAL, &timer, nullptr)) {
        cerr << TIMER_ERR_MSG << endl;
        return -1;
    }
    return 0;
}
/*
 * Description: This function initializes the Thread library.
 * You may assume that this function is called before any other Thread library
 * function, and that it is called exactly once.
*/
int uthread_init(int quantum_usecs){
    //create mask to block signals() //tODO
    if (quantum_usecs < 0)
    {
        cerr << SYS_ERR_MSG << INVALID_QUANTOM_MSG << endl;
        return -1;
    }

    int setTimer = setTimerSignalHandler(quantum_usecs);
    assert(setTimer != -1);

    int mainThread = uthread_spawn(nullptr); // init mainThread
    totalSizeOfQuantums++;
    assert(mainThread != -1);

    //unblock signals() //tODO
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
        cerr << THREAD_LIB_ERR_MSG << MAX_THREAD_ERR_MSG << endl;
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
        if (allThreadList[i].id == threadId) return (int)i;
    }
    cerr << THREAD_NOT_FOUND_MSG << endl;
    return -1;
}

//Runs on ptrVectorList
int findThreadPtrById(int threadId, vector<Thread*>& vector1){
    assert(threadId >= 0);
    for(vector<Thread>::size_type i = 0; i != vector1.size(); i++) {
        if (vector1[i]->id == threadId) return (int)i;
    }
    return -1;
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
        cerr << THREAD_LIB_ERR_MSG << BAD_TID_ERR_MSG << endl;
        return -1;
    }
    if (tid == 0){
        cerr << TERMINATING_MAIN_THREAD_MSG <<  endl;
        exit(0);
    }

    State threadState = allThreadList[tid].state;
    if (threadState == READY || threadState == RUNNING){
        int threadIdx = findThreadPtrById(tid, readyThreadPtrList);
        readyThreadPtrList.erase(readyThreadPtrList.begin() + threadIdx);
    }
    else if (threadState == BLOCKED){
        int threadIdx = findThreadPtrById(tid, blockedThreadPtrList);
        blockedThreadPtrList.erase(blockedThreadPtrList.begin() + threadIdx);
    }

    globalThreadCounter--;

    return 0;
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
int uthread_block(int tid){
    if (tid < 0) {
        cerr << THREAD_LIB_ERR_MSG << BAD_TID_ERR_MSG << endl;
        return -1;
    }
    if (tid == 0){
        cerr << THREAD_LIB_ERR_MSG << MAIN_THREAD_BLOCK_ERR_MSG << endl;
        return -1;
    }
    allThreadList[tid].state = BLOCKED;


}


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

