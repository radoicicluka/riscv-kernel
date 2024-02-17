//
// Created by os on 6/13/23.
//

#include "../h/pcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

PCB *PCB::running = nullptr;

PCB *PCB::createThread(PCB::Body body, void* args) {
    return new PCB(body, args);
}

void PCB::yield(){
    Riscv::pushRegisters();
    PCB::dispatch();
    Riscv::popRegisters();
}

void PCB::dispatch() {

    PCB* old = running;
    if(old->waitingFor == 0 && !old->blocked)Scheduler::put(old);
    PCB* next_thread = Scheduler::get();
    while (next_thread->isFinished()){
        PCB* to_delete = next_thread;
        next_thread = Scheduler::get();
        to_delete->startAllBlockedThreads();
        MemoryAllocator::instance()._mem_free(to_delete);
    }
    running = next_thread;
    PCB::contextSwitch(&old->context, &running->context);

}

void PCB::threadWrapper() {
    Riscv::popSppSpie(); // iz koda prekidne rutin se skace u user mode
    if (running->body) running->body(running->args);
    thread_exit();
}

bool PCB::restartable(){
    if (PCB::waitingFor == 0) return true;
    return PCB::waitingFor->isFinished();
}

void PCB::startAllBlockedThreads(){
    while(PCB::blockedThreads.peekFirst() != 0){
        if(PCB::blockedThreads.peekFirst()->restartable()){
            PCB* pcb = PCB::blockedThreads.removeFirst();
            if (pcb != 0){
                pcb->waitingFor = 0;
                Scheduler::put(pcb);
            }
        }
    }
}

void PCB::join(PCB* handle){
    if (handle == 0) return;
    if (handle == running) return; // poziv za samu sebe
    if (handle->isFinished()) return;
    if (handle->blockedThreads.contains(running)) return;
    handle->blockedThreads.addLast(running);
    PCB::waitingFor = handle;
    yield();
}