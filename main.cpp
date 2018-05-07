#include <iostream>
#include "uthreads.h"
#include <vector>
using namespace std;
int a = 0;
//int b = 0;
void halt()
{
    while (true)
    {}
}

void thread1(){
    cout << "in t1" << endl;

    while (true)
    {
        if (a == 3){
            cout << "t1" << endl;
        }
        a = 0;
    }
}
void thread2()
{
    cout << "in t2" << endl;
    while (true)
    {
        if (a == 0){
            cout << "t2" << endl;
        }
        a = 1;
    }
}



int main()
{
    uthread_init(2000000); //mainThread
    uthread_spawn(thread1); // [mainThread, thread1]
   // uthread_spawn(thread2); // [t0, t1, t2]




    std::cout << "Hello, World!" << std::endl;
    while (true) {
        if (a == 0){
            cout << "main" << endl;
        }
        a = 3;
    }
    return 0;
}
