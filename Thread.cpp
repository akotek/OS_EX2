//
// Created by yoavabadi on 5/7/18.
//

#include "Thread.h"

#define START_QUANTA_VALUE 0

class Thread
{
public:
    Thread(int id, int stackSize, functionPointer f) : _id(id), _functionPointer(f),
                                                       _quantums(START_QUANTA_VALUE), _state(READY)

    {
    }

    ~Thread();

private:
    int _id;
    State _state;
    functionPointer _functionPointer;
    char * _stack;
    int _quantums;
    sigjmp_buf * _env;
};
