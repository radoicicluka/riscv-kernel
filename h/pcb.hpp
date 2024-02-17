//
// Created by os on 6/13/23.
//

#ifndef PROJECT_BASE_PCB_HPP
#define PROJECT_BASE_PCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Programme Control Block
class PCB{
public:
    ~PCB(){
        delete[] stack;
    }

    bool isFinished() const {
        return finished;
    }

    void setFinished(bool finished) {
        PCB::finished = finished;
    }

    using Body = void (*)(void *);

    static PCB* createThread(Body body, void* args);

    static void yield();

    static PCB* running;

    bool blocked;

private:
    PCB(Body body, void* args) :
            body(body),
            stack((body != nullptr)?new uint64[STACK_SIZE] : nullptr),
            context({
                (uint64) &threadWrapper,
                (stack != nullptr)? (uint64)&stack[STACK_SIZE] : 0
            }),
            finished(false),
            waitingFor(0)
    {
        this->args = args;
        blocked = false;
    }

    struct Context{
        uint64 ra; // return address
        uint64 sp;
    };
    Body body;
    uint64* stack; // niz 64-bit vrednosti
    Context context;
    bool finished;

    void* args;

    PCB* waitingFor;
    List<PCB> blockedThreads;

    friend class Riscv; // Riscv == "masina"

    static void threadWrapper();

    static void contextSwitch(Context* oldContext, Context* runningContext);

    static void dispatch();

    static uint64 constexpr STACK_SIZE = 1024;

    bool restartable(); // proverava da li je zavrsena nit koju je this nit cekala

    void startAllBlockedThreads(); // vraca u scheduler sve niti koje su cekale na this nit i restartable su

    void join(PCB* handle);

};

#endif //PROJECT_BASE_PCB_HPP
