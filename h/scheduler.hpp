//
// Created by os on 6/13/23.
//

#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "list.hpp"

class PCB;

class Scheduler{
private:
    static List<PCB> readyProcessQueue;

public:
    static PCB *get();

    static void put(PCB *pcb);

    static int len(){
        return readyProcessQueue.len();
    }
};

#endif //PROJECT_BASE_SCHEDULER_HPP
