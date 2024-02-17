//
// Created by os on 7/4/23.
//

#include "../h/riscv.hpp"
#include "../h/pcb.hpp"
#include "../lib/console.h"
#include "../h/memoryAllocator.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/sem.hpp"

void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    mc_sstatus(SSTATUS_SPP);
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap(){
    uint64 scause = r_scause();
    if (scause == 0x0000000000000009UL || scause == 0x0000000000000008UL){
        // interrupt: NO, cause code: enviroment call from S-mode (9) or U-mode(8)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 volatile caller;
        __asm__ ("mv %0, a0" : "=r" (caller));
        if (caller == 0x01){ // mem_alloc
            size_t sizeOfNewMemory;
            __asm__ volatile ("mv %0, a1" : "=r" (sizeOfNewMemory));
            void* newAddress = MemoryAllocator::instance()._mem_alloc(sizeOfNewMemory);
            __asm__ volatile ("mv a0, %0" : : "r"(newAddress));
        }
        else if (caller == 0x02){ // mem_free
            void* oldAddress;
            __asm__ volatile ("mv %0, a1" : "=r" (oldAddress));
            int volatile retValue = MemoryAllocator::instance()._mem_free(oldAddress);
            __asm__ volatile ("mv a0, %0" : : "r"(retValue));
        }
        else if (caller == 0x11){ // create_thread
            PCB::Body foo;
            void* arg;
            PCB ** handle;
            __asm__ ("mv %0, a6" : "=r" (arg));
            __asm__ ("mv %0, a2" : "=r" (foo));
            __asm__ ("mv %0, a1" : "=r" (handle));
            PCB* pcb = new PCB(foo, arg);
            int ret = 0;
            if (pcb == 0) ret = 1;
            else{
                *handle = pcb;
//                if(foo != nullptr)
                Scheduler::put(pcb);
            }
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x12){ // thread_exit
            int ret = 0;
            PCB::running->setFinished(true);
            PCB::running->startAllBlockedThreads();
//            delete PCB::running;
            PCB::yield();
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x13){ // thread_dispatch
            PCB::yield();
        }
        else if (caller == 0x14){ // thread_join
            PCB* handle;
            __asm__ volatile ("mv %0, a1" : "=r" (handle));
            PCB::running->join(handle);
        }
        else if (caller == 0x15){ // Thread() i Thread(void (*foo)(void*), void* arg)
            PCB::Body volatile foo;
            void* volatile arg;
            PCB ** volatile handle;
            __asm__ volatile ("mv %0, a6" : "=r" (arg));
            __asm__ volatile ("mv %0, a2" : "=r" (foo));
            __asm__ volatile ("mv %0, a1" : "=r" (handle));
            PCB* pcb = new PCB(foo, arg);
            *handle = pcb;
        }
        else if (caller == 0x16){ // Thread::start()
            PCB* handle;
            __asm__ volatile ("mv %0, a1" : "=r" (handle));
            if (handle != 0) Scheduler::put(handle);
        }
        else if (caller == 0x19){ // ~Thread()
            PCB* handle;
            bool isRunning = false;
            __asm__ volatile ("mv %0, a1" : "=r" (handle));
            if (handle != 0){
                if (!handle->isFinished()){
                    if (handle == PCB::running) isRunning = true;
                    handle->setFinished(true);
                    handle->startAllBlockedThreads();
//                    delete handle;
                    if (isRunning) {
                        PCB::yield();
                    }
                }
            }
        }
        else if (caller == 0x21){ // sem_open
            unsigned init;
            int ret = 0;
            Sem ** handle;
            __asm__ ("mv %0, a2" : "=r" (init));
            __asm__ ("mv %0, a1" : "=r" (handle));
            Sem* sem = new Sem(init);
            if (sem == 0) ret = -1;
            *handle = sem;
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x22){ // sem_close
            int ret = 0;
            Sem* handle;
            __asm__ ("mv %0, a1" : "=r" (handle));
            handle->startAll();
//            delete handle;
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x23){ // sem_wait
            int ret = 0;
            Sem* handle;
            __asm__ ("mv %0, a1" : "=r" (handle));
            if (handle == 0 || PCB::running == 0) ret = -1;
            else handle->wait();
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x24){ // sem_signal
            int ret = 0;
            Sem* handle;
            __asm__ ("mv %0, a1" : "=r" (handle));
            if (handle == 0 || PCB::running == 0) ret = -1;
            else handle->signal();
            __asm__ volatile ("mv a0, %0" : : "r"(ret));
        }
        else if (caller == 0x41){
            char c = __getc();
            __asm__ volatile ("mv a0, %0" : : "r"((uint64)c));
        }
        else if (caller == 0x42){
            char c;
            __asm__ volatile ("mv %0, a1" : "=r" (c));
            __putc(c);
        }

//        __asm__ volatile ("mv a0, %0" : : "r"(ret));
//        __asm__ volatile ("csrr %0, sscratch" : "=r"(sp));
//        __asm__ volatile ("sd a0, 10 * 8(%0)" : : "r"(sp));
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes, cause code: supervisor software interrupt (timer)
        mc_sip(SIP_SSIP);

    } else if (scause == 0x8000000000000009UL) {
        // interrupt: yes, cause code: supervisor external interrupt (console)
        console_handler();
    } else {
        // unexpected trap cause
        // scause, sepc, stval
    }
}