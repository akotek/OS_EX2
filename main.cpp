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
    cout << "t1" << endl;
    printf("t1");
    halt();
}
void thread2()
{
    cout << "t2" << endl;
    halt();
}



int main()
{
    uthread_init(3000000);
    uthread_spawn(thread1);
    uthread_spawn(thread2);



//    uthread_terminate(2);

    std::cout << "Hello, World!" << std::endl;
    for(;;) {}
    return 0;
}
