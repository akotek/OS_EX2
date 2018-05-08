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

    ~Thread()
    {

    }
    int _id;
    State _state;
    functionPointer _functionPointer;
    char * _stack;
    int _quantums;
    sigjmp_buf * _env;
};
//
//
////TODO switchthreads= Terminate, Block, Sync
//// data structures:
//// ----------------
//struct Thread {
//    int id;
//    char* stack;
//    sigjmp_buf env{};
//    int quantomsRanByThread;
//    int syncThreadId = NOT_SYNC;
//
//    Thread(int id){
//        this->id = id;
//        this->stack = new char[STACK_SIZE];
////        this->state = READY;
//        this->quantomsRanByThread = 0;
//    }
//};
