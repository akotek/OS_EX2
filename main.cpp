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
    cout << "in t1" << endl;
    halt();
}
void thread2()
{
    cout << "in t2" << endl;
    halt();
}



int main()
{
    uthread_init(2000000);
    uthread_spawn(thread1);
    uthread_spawn(thread2);


    std::cout << "Hello, World!" << std::endl;
    for(;;) {}
    return 0;
}
