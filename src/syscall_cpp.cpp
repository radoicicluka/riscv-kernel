//
// Created by os on 8/12/23.
//

#include "../h/syscall_cpp.hpp"
#include "../h/scheduler.hpp"

void* operator new (size_t size){
    return mem_alloc(size);
}

void operator delete(void* addr){
    int result = mem_free(addr);
    if(result) return;
}

void *operator new[](size_t size){
    return mem_alloc(size);
}

void operator delete[](void *addr) noexcept{
    int result = mem_free(addr);
    if(result) return;
}

Thread::Thread (void (*body)(void*), void* arg){
    size_t hex = 0x15;
    myHandle = 0;
    this->body = body;
    this->arg = arg;
    __asm__ ("mv a3, %0" : : "r" (arg));
    __asm__ ("mv a2, %0" : : "r" (body));
    __asm__ ("mv a1, %0" : : "r" (&myHandle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

int Thread::start() {
    size_t hex = 0x16;
    int ret;
    __asm__ ("mv a1, %0" : : "r" (myHandle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ ("mv %0, a0" : "=r" (ret));
    return ret;
}

Thread::Thread() {
    size_t hex = 0x15;
    myHandle = 0;
    this->body = &Thread::runWrapper;
    this->arg = (void*)this;
    __asm__ ("mv a6, %0" : : "r" (arg));
    __asm__ ("mv a2, %0" : : "r" (body));
    __asm__ ("mv a1, %0" : : "r" (&myHandle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

Thread::~Thread() {
    size_t hex = 0x19;
    __asm__ ("mv a1, %0" : : "r" (myHandle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

void Thread::join() {
    thread_join(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t) {
    return 0;
}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}

void PeriodicThread::terminate() {

}

PeriodicThread::PeriodicThread(time_t period) {

}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
