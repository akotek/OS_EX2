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

// global/static variables
// ----------------
int globalThreadCounter = 1;
int quantumSizeUsec;
int totalSizeOfQuantums;
vector <Thread> allThreadList;
vector <Thread*> readyList; // represents readyQueue
vector <Thread*> blockedList; // represents blockedListOfThreads
// ----------------

// data structures:
// ----------------
struct Thread {
    int id;
    State state;
    sigjmp_buf env{};
    //vector <Thread*> dependencyThList;

    Thread(int id, State state){
        this->id = id;
        this->state = state;
    }

};
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

Thread initThread(void (*f), int threadId){
    address_t sp, pc;
    char stack1[STACK_SIZE];

    Thread newTh (threadId, READY);
    sp = (address_t)stack1 + STACK_SIZE - sizeof(address_t);
    pc = (address_t)f;

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
    // create new Thread - allocate control block
    // init Thread control block and stack with register value and PC
    // tell dispatcher that it can run the Thread: put Thread in end of ready
    // list
    struct Thread thread1 = initThread(f, globalThreadCounter);
    globalThreadCounter++;
    allThreadList.push_back(thread1);
    readyList.push_back(&(allThreadList[allThreadList.size()]));


}
