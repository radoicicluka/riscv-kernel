//
// Created by os on 6/13/23.
//
#include "../h/pcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/sem.hpp"


extern void userMain();

void userMainWrapper(void*){
    userMain();
}

int main(){
    Riscv::w_stvec((uint64)&Riscv::supervisorTrap); // postavljanje adrese prekidne rutine
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE); // omogucavanje prekida globalno

    PCB* pcb;
    thread_create(&pcb, nullptr, 0);
    PCB::running = Scheduler::get();
    PCB* userMain;
    thread_create(&userMain, userMainWrapper, 0);
    thread_join(userMain);

    return 0;
}