#include <cstdio>
#include <csetjmp>
#include <csignal>
#include <cassert>
#include <vector>
#include <sys/time.h>
#include <memory>
#include <iostream>
#include <list>
#include "uthreads.h"
using namespace std;

//TODO: -delete asserts after

// Constants and DEFINES:
// ----------------
enum State {NOT_ACTIVE, RUNNING, READY, BLOCKED};
typedef unsigned long address_t;
sigset_t set, pending;

#define JB_SP 6
#define JB_PC 7
#define NOT_SYNC -1
#define USEC_TO_MICRO 1000000
#define SYS_ERR_MSG "system error: "
#define THREAD_LIB_ERR_MSG "thread library error: "
#define INVALID_QUANTOM_MSG "invalid quantum size"
#define SIGACTION_ERR_MSG "sigaction error."
#define TIMER_ERR_MSG "setitimer error."
#define MAX_THREAD_ERR_MSG "too many threads, can't create new thread."
#define BAD_TID_ERR_MSG "invalid tid."
#define TERMINATING_MAIN_THREAD_MSG "terminating main thread, existing process"
#define THREAD_NOT_FOUND_MSG "thread not found in thread list"
#define MAIN_THREAD_BLOCK_ERR_MSG "error trying to block main thread"
// ----------------

// data structures:
// ----------------
struct Thread {
    int id;
    char* stack;
    sigjmp_buf env{};
    int quantomsRanByThread;
    list<int> syncThreadId;

    Thread(int id){
        this->id = id;
        this->stack = new char[STACK_SIZE];
//        this->state = READY;
        this->quantomsRanByThread = 0;
    }
    ~Thread(){}
};

// ----------------

// global/static variables
// ----------------
static int globalThreadCounter = 0;
static int totalSizeOfQuantums = 0;
struct sigaction sa;
struct itimerval timer;

static vector <Thread> threadsVector;
static State threadsState[MAX_THREAD_NUM];
static vector<int> readyThreadQueue;
static int globalQuanta = 0;
static int gRunningThread = 0;

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

    // returns the next smallest unused thread id
    int findNextThreadId()
    {
        for(int idx=0; idx<MAX_THREAD_NUM; idx++)
        {
            if(threadsState[idx] == NOT_ACTIVE)
            {
                return idx;
            }
        }
    }

    int findReadyThreadById(int threadId)
    {
        assert(threadId >= 0);
        for(vector<int>::size_type i = 0; i != readyThreadQueue.size(); i++) {
            if (readyThreadQueue[i] == threadId) return (int)i;
        }
        cerr << THREAD_NOT_FOUND_MSG << endl;
        return -1;
    }

}

// ----------------

void setBlockedSignal(){
//    sigset_t set;
//    sigemptyset(&set);
//    sigaddset(&set, SIGVTALRM);
//    sigprocmask(SIG_BLOCK, &set, nullptr);
    if(sigprocmask(SIG_BLOCK, &set, NULL) == -1)
    {
        cerr << TIMER_ERR_MSG << endl;
        exit(-1);
    }
//    sigpending(&pending);

}
void unblockSignal(){
//    sigemptyset(&set);
//    sigaddset(&set, SIGVTALRM);
    if(sigprocmask(SIG_UNBLOCK, &set, NULL))
    {
        cerr << TIMER_ERR_MSG << endl;
        exit(-1);
    }
}
int setTimerSignalHandler(int quantomUsecs){

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



void resumeSyncThread(int tid)
{

    setBlockedSignal();
    for(int &syncId : threadsVector[tid].syncThreadId)
    {
        uthread_resume(syncId);

    }
    threadsVector[tid].syncThreadId.clear();
    unblockSignal();
}


Thread initThread(void (*f)(void), int threadId){
    setBlockedSignal();
    address_t sp, pc;

    Thread newThread (threadId);
    sp = (address_t)newThread.stack + STACK_SIZE - sizeof(address_t);
    pc = (f != nullptr ? (address_t)f : 0); // 0 if mainThread -TODO: maybe it not contradict vector
    sigsetjmp(newThread.env, 1);
    (newThread.env->__jmpbuf)[JB_SP] = helperFuncs::translate_address(sp);
    (newThread.env->__jmpbuf)[JB_PC] = helperFuncs::translate_address(pc);
    sigemptyset(&newThread.env->__saved_mask);

    unblockSignal();
    return newThread;
}

void switchThreads(){


    setBlockedSignal();

    if (threadsState[gRunningThread] == RUNNING){
        threadsState[gRunningThread] = READY;
        readyThreadQueue.erase(readyThreadQueue.begin());
        readyThreadQueue.push_back(gRunningThread);
    }

    int ret_val = sigsetjmp(threadsVector[gRunningThread].env,1);
    if (ret_val == 1) {
        unblockSignal();
        setTimerSignalHandler(globalQuanta);
        return;
    }


    gRunningThread = readyThreadQueue[0];
    threadsState[gRunningThread] = RUNNING;

    threadsVector[gRunningThread].quantomsRanByThread++;

    totalSizeOfQuantums++;
    unblockSignal();
    siglongjmp(threadsVector[gRunningThread].env,1);
}

void signalHandler(int signal){
    switchThreads();
}



//void mainThreadFunc()
//{
//    cout << "main thread" << endl;
//}

/*
 * Description: This function initializes the Thread library.
 * You may assume that this function is called before any other Thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs){
    globalQuanta = quantum_usecs;
    if (quantum_usecs < 0)
    {
        cerr << SYS_ERR_MSG << INVALID_QUANTOM_MSG << endl;
        return -1;
    }

    // Install RR algorithm as the signal handler for SIGVTALRM:
    sa.sa_handler = &signalHandler;
    if (sigaction(SIGVTALRM, &sa, nullptr) < 0) {
        cerr << SIGACTION_ERR_MSG << endl;
    }
    int setTimer = setTimerSignalHandler(quantum_usecs);

    int mainThread = uthread_spawn(nullptr); // init mainThread
    threadsState[0] = RUNNING;
    threadsVector[0].quantomsRanByThread++;
    globalThreadCounter++;
    totalSizeOfQuantums++;

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
    setBlockedSignal();

//    assert(f != nullptr);
    if (threadsVector.size() > MAX_THREAD_NUM)    {
        cerr << THREAD_LIB_ERR_MSG << MAX_THREAD_ERR_MSG << endl;
        return -1;
    }


    int threadId = helperFuncs::findNextThreadId();
    struct Thread spawnedThread = initThread(f, threadId);
    threadsState[threadId] = READY;
    readyThreadQueue.push_back(threadId);
    threadsVector.insert(threadsVector.begin() + threadId, spawnedThread);
    globalThreadCounter +=1;

    unblockSignal();
    return spawnedThread.id;
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
    setBlockedSignal();
    if (tid < 0)
    {
        cerr << THREAD_LIB_ERR_MSG << BAD_TID_ERR_MSG << endl;
        return -1;
    }
    if (tid == 0){
        cout << TERMINATING_MAIN_THREAD_MSG <<  endl; // it's not an error
        vector<Thread>().swap(threadsVector); // releasing memory by swapping to an empty vector
        exit(0);
    }
    State tmpState = threadsState[tid];
    resumeSyncThread(tid);
    threadsState[tid] = NOT_ACTIVE;
    if (tmpState != BLOCKED)
    {

        int idx = helperFuncs::findReadyThreadById(tid); // TODO: create inner func
        readyThreadQueue.erase(readyThreadQueue.begin() + idx);

    }
    if (tmpState == RUNNING)
    {
        gRunningThread = tid;
        switchThreads();
    }

    globalThreadCounter--;
    unblockSignal();
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
    setBlockedSignal();
    if (tid < 0) {
        cerr << THREAD_LIB_ERR_MSG << BAD_TID_ERR_MSG << endl;
        return -1;
    }
    if (tid == 0){
        cerr << THREAD_LIB_ERR_MSG << MAIN_THREAD_BLOCK_ERR_MSG << endl;
        return -1;
    }

    State tmpState = threadsState[tid];
    threadsState[tid] = BLOCKED;
    if (tmpState == READY)
    {
        int idx = helperFuncs::findReadyThreadById(tid);
        readyThreadQueue.erase(readyThreadQueue.begin()+idx);
    }


    if (tmpState == NOT_ACTIVE)
    {
        cerr << THREAD_LIB_ERR_MSG << THREAD_NOT_FOUND_MSG << endl;
        return -1;
    }
    if (tmpState == RUNNING)
    {
        gRunningThread = tid;
        switchThreads();
    }


    unblockSignal();
    return 0;
}


/*
 * Description: This function resumes a blocked Thread with ID tid and moves
 * it to the READY state. Resuming a Thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no Thread with
 * ID tid exists it is considered as an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
//    printf("RESUMES : %d\n", tid);
    setBlockedSignal();
    if (tid < 0) {
        cerr << THREAD_LIB_ERR_MSG << BAD_TID_ERR_MSG << endl;
        return -1;
    }
    if (threadsState[tid] == NOT_ACTIVE)
    {
        cerr << THREAD_LIB_ERR_MSG << THREAD_NOT_FOUND_MSG << endl;
        return -1;
    }


//        printf("unblocked: %d\n", tid);
    threadsState[tid] = READY;
    readyThreadQueue.push_back(tid);

//    printf("current Running: %d\n", readyThreadQueue[0]);
//    setTimerSignalHandler(globalQuanta);
    unblockSignal();
    return 0;
}


/*
This function blocks the RUNNING thread until thread with ID tid terminates (i.e. right after
uthread_termiante(tid) is called). It is considered an error if no thread with ID tid exists or if the main thread
(tid==0) calls this function. Immediately after the RUNNING thread transitions to the BLOCKED state a
scheduling decision should be made.
Return value: On success, return 0. On failure, return -1.
*/
int uthread_sync(int tid)
{
    setBlockedSignal();
    if(tid == readyThreadQueue[0])
    {
        cout << SYS_ERR_MSG << endl; // TODO: write error msg
        return -1;
//        printf("RUNNING THREAD\n"); // TODO: Should we handle this?
    }
    int runningThreadId = readyThreadQueue[0];

    threadsVector[tid].syncThreadId.push_back(runningThreadId);
    int block = uthread_block(runningThreadId);
//    switchThreads();
    unblockSignal();
    return block;

//    && threadsVector[tid].syncThreads.empty()) // checks if the
//    // thread's blocked by another process

}


/*
 * Description: This function returns the Thread ID of the calling Thread.
 * Return value: The ID of the calling Thread.
*/
int uthread_get_tid()
{
//    setBlockedSignal();
//    cout << "currentRunningThread : " << readyThreadQueue[0] << endl;
//    unblockSignal();
//    assert(threadsState[readyThreadQueue[0]] == RUNNING);
    if(threadsState[readyThreadQueue[0]] != RUNNING)
    {
//        return 0;
        printf("ERROR\n");
    }
    return readyThreadQueue[0];

}


/*
 * Description: This function returns the total number of quantums that were
 * started since the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{

    return totalSizeOfQuantums;
}


/*
 * Description: This function returns the number of quantums the Thread with
 * ID tid was in RUNNING state. On the first time a Thread runs, the function
 * should return 1. Every additional quantum that the Thread starts should
 * increase this value by 1 (so if the Thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * Thread with ID tid exists it is considered as an error.
 * Return value: On success, return the number of quantums of the Thread with ID tid. On failure, return -1.
*/
int uthread_get_quantums(int tid)
{
    setBlockedSignal();
    if(tid < 0 || threadsState[tid] == NOT_ACTIVE)
    {
        return -1;
    }
    if(threadsState[tid] == RUNNING)
    {
        return threadsVector[tid].quantomsRanByThread++;
    }

    unblockSignal();
    return threadsVector[tid].quantomsRanByThread;
}

