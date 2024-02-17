//
// Created by os on 9/11/23.
//

#include "../h/sem.hpp"
#include "../h/pcb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/scheduler.hpp"

void Sem::wait() {
    value--;
    bool is = value < 0;
    if (is) block();
}

void Sem::signal() {
    value++;
    bool is = value <= 0;
    if (is) unblock();
}

void Sem::block() {
    PCB::running->blocked = true;
    blocked.addLast(PCB::running);
    PCB::yield();
}

void Sem::unblock() {
    PCB* pcb = blocked.removeFirst();
    pcb->blocked = false;
    Scheduler::put(pcb);
}

void Sem::startAll(){
    while(Sem::blocked.peekFirst() != 0){
        unblock();
    }
}
