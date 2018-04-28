#include <cstdio>
#include <csetjmp>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include "uthreads.h"
using namespace std;

// Constants:
// ----------------
enum State {READY, WAITING, BLOCKED};
#define JB_SP 6
#define JB_PC 7
typedef unsigned long address_t;
// ----------------

// data structures:
// ----------------
struct Thread {
    int id;
    State state;
    sigjmp_buf env{};
    //vector <int> dependencyThList;

    Thread(int id, State state){
        this->id = id;
        this->state = state;
    }

};
// ----------------

// global/static variables
// ----------------
int globalThreadCounter = 1;
int quantumSizeUsec;
int totalSizeOfQuantums;
vector <Thread> allThreadList;
vector <Thread*> readyList; // represents readyQueue
vector <Thread*> blockedList; // represents blockedListOfThreads
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

/*
 * Description: This function initializes the Thread library.
 * You may assume that this function is called before any other Thread library
 * function, and that it is called exactly once.
*/
int uthread_init(int quantum_usecs){
    if (quantum_usecs < 0) return -1;
    quantumSizeUsec = quantum_usecs;

}

Thread initThread(void (*f)(void), int threadId=globalThreadCounter){
    address_t sp, pc;
    char stack1[STACK_SIZE];

    Thread newTh (threadId, READY);
    sp = (address_t)stack1 + STACK_SIZE - sizeof(address_t);
    pc = (address_t)f; // sets thread to work on func f

    sigsetjmp(newTh.env, 1);
    (newTh.env->__jmpbuf)[JB_SP] = helperFuncs::translate_address(sp);
    (newTh.env->__jmpbuf)[JB_PC] = helperFuncs::translate_address(pc);
    sigemptyset(&newTh.env->__saved_mask);

    return newTh;
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

    struct Thread thread1 = initThread(f);
    globalThreadCounter++;

    allThreadList.push_back(thread1);
    Thread* thread1Ptr = &(allThreadList[allThreadList.size()]);
    readyList.push_back(thread1Ptr);

    if (allThreadList.size() > MAX_THREAD_NUM) return -1;
    return thread1.id;
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
int uthread_terminate(int tid);


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

