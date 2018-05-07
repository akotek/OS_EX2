//
// Created by yoavabadi on 5/7/18.
//

#ifndef EX2_THREAD_H
#define EX2_THREAD_H
#include <signal.h>
#include <setjmp.h>

typedef void (*functionPointer)(void);
enum State {NOT_ACTIVE, RUNNING, READY, BLOCKED};

class Thread
{
public:
    Thread(int id, int stackSize, functionPointer f);

    ~Thread();

private:
    int _id;
    State _state;
    functionPointer _functionPointer;
    char * _stack;
    int _quantums;
    sigjmp_buf * _env;
};


#endif //EX2_THREAD_H
