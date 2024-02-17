//
// Created by os on 9/11/23.
//

#ifndef PROJECT_BASE_SEM_HPP
#define PROJECT_BASE_SEM_HPP

#endif //PROJECT_BASE_SEM_HPP

#include "../h/list.hpp"
#include "../h/pcb.hpp"

class Sem {
public:
    Sem(unsigned init = 1) : value(init){}

    void wait();
    void signal();

protected:
    void block();
    void unblock();

private:
    int value;
    List<PCB> blocked;
    void startAll();
    friend class Riscv;
};