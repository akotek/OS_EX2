#include <iostream>
#include "uthreads.h"
#include <vector>
using namespace std;
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



//int main()
//{
//    uthread_init(30);
//    uthread_spawn(thread1);
//    uthread_spawn(thread2);
//    uthread_terminate(2);
//
//
//    std::cout << "Hello, World!" << std::endl;
//    return 0;
//}
