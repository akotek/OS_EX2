#include <iostream>
#include "uthreads.h"

void halt()
{
    while (true)
    {}
}

void thread1(){
    printf("t1");
}
void thread2()
{
    halt();
}


int main()
{

    uthread_spawn(thread1);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}