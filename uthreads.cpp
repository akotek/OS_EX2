#include "uthreads.h"

//TODO: do we need to make it a class?

// Data members:
// ----------------
int quantom_size_usecs;

// ----------------

/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once.
*/
int uthread_init(int quantum_usecs){
    if (quantum_usecs < 0) return -1;

    quantom_size_usecs = quantum_usecs;

}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void)){

}
