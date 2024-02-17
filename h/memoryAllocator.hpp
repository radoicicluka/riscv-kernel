//
// Created by os on 8/12/23.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    struct FreeMem {
        FreeMem* next;
        size_t size;
    };
private:

    FreeMem *freeMemHead;

    MemoryAllocator();

    void compact(FreeMem* cur);

public:

    static MemoryAllocator& instance();

    void* _mem_alloc(size_t size);

    int _mem_free(void* addr);

};
#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
