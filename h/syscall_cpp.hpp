//
// Created by os on 8/12/23.
//

#ifndef PROJECT_BASE_SYSCALL_CPP_HPP
#define PROJECT_BASE_SYSCALL_CPP_HPP

#include "syscall_c.hpp"

void* operator new (size_t size);
void operator delete(void* addr);

void *operator new[](size_t size);
void operator delete[](void *addr) noexcept;

class Thread{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread();

    int start();

    void join();

    static void dispatch();
    static int sleep(time_t);

    thread_t getHandle(){
        return myHandle;
    }

protected:
    Thread();
    virtual void run() {};

private:
    thread_t myHandle;
    void (*body)(void*); void* arg;

    static void runWrapper(void* threadToRun){
        ((Thread*)threadToRun)->run();
    }
};


class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};


class PeriodicThread : public Thread {
public:
    void terminate ();
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    time_t period;
};


class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif //PROJECT_BASE_SYSCALL_CPP_HPP
