//
// Created by os on 8/12/23.
//

#include "../h/syscall_c.hpp"

void* mem_alloc(size_t size){
    // size je u bajtovima
    size_t sizeWithHeader = size + sizeof(MemoryAllocator::FreeMem);
    size_t sizeInBlocks = sizeWithHeader / MEM_BLOCK_SIZE;
    bool whole = sizeWithHeader % MEM_BLOCK_SIZE == 0;
    if (!whole) sizeInBlocks++;
    void* addr = 0;
    size_t hex = 0x01;
    __asm__ ("mv a1, %0" : : "r" (sizeInBlocks));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (addr));

    return addr;
}

int mem_free(void* addr){
    size_t hex = 0x02;
    int msg;
    __asm__ ("mv a1, %0" : : "r" (addr));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (msg));
    return msg;
}

int thread_create (thread_t * handle, void(*start_routine)(void*), void* arg){
    size_t hex = 0x11;
    void* ret;
    __asm__ ("mv a6, %0" : : "r" (arg));
    __asm__ ("mv a2, %0" : : "r" (start_routine));
    __asm__ ("mv a1, %0" : : "r" (handle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    if (ret) return 0;
    return -1;
}

void thread_dispatch (){
    size_t hex = 0x13;
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

int thread_exit (){
    size_t hex = 0x12;
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    int ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

void thread_join(thread_t handle){
    size_t hex = 0x14;
    __asm__ ("mv a1, %0" : : "r" (handle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

char getc(){
    size_t hex = 0x41;
    uint64 c;
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (c));
    return (char)c;
}

void putc(char c){
    size_t hex = 0x42;
    __asm__ ("mv a1, a0");
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
}

int sem_open (sem_t* handle, unsigned init){
    size_t hex = 0x21;
    int ret;
    __asm__ ("mv a2, %0" : : "r" (init));
    __asm__ ("mv a1, %0" : : "r" (handle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// 0x22
int sem_close (sem_t handle){
    size_t hex = 0x22;
    int ret;
    __asm__ ("mv a1, %0" : : "r" (handle));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// 0x23
int sem_wait (sem_t id){
    size_t hex = 0x23;
    int ret;
    __asm__ ("mv a1, %0" : : "r" (id));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// 0x24
int sem_signal (sem_t id){
    size_t hex = 0x24;
    int ret;
    __asm__ ("mv a1, %0" : : "r" (id));
    __asm__ ("mv a0, %0" : : "r" (hex));
    __asm__ ("ecall");
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// 0x31
typedef unsigned long time_t;

int time_sleep (time_t){
    return 0;
}
