//
// Created by os on 6/13/23.
//

#include "../h/scheduler.hpp"

List<PCB> Scheduler::readyProcessQueue;

PCB* Scheduler::get(){
    return readyProcessQueue.removeFirst();
}

void Scheduler::put(PCB* pcb){
    readyProcessQueue.addLast(pcb);
}

