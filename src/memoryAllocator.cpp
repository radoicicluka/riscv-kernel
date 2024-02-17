//
// Created by os on 8/12/23.
//

#include "../h/memoryAllocator.hpp"

MemoryAllocator::MemoryAllocator() {
    MemoryAllocator::freeMemHead = (MemoryAllocator::FreeMem*)((size_t)HEAP_START_ADDR + sizeof(MemoryAllocator::FreeMem));
    MemoryAllocator::freeMemHead->size = ((size_t)HEAP_END_ADDR - (size_t)HEAP_START_ADDR - (size_t)MemoryAllocator::freeMemHead);
}

MemoryAllocator& MemoryAllocator::instance() {
    static MemoryAllocator ins;
    return ins;
}

void* MemoryAllocator::_mem_alloc(size_t size) { // velicina u blokovima ne u bajtovima - uracunato sa velicinom zaglavlja
    if (!freeMemHead) return 0;

    FreeMem* cur = freeMemHead;
    FreeMem* prev = 0;

    for (; cur != 0; prev = cur, cur = cur->next){
        if (cur->size < size * MEM_BLOCK_SIZE) continue;
        if (cur->size == size * MEM_BLOCK_SIZE){
            // nema fragmentacije -> cvor je iste velicine u blokovima kao i size
            if (prev) prev->next = cur->next;
            else freeMemHead = cur->next;
        } else {
            // fragmentacija -> cvor je vece velicine od size
            FreeMem* fragment = (FreeMem*)((size_t)cur + size * MEM_BLOCK_SIZE);
            if (prev) prev->next = fragment;
            else freeMemHead = fragment;
            fragment->next = cur->next;
            fragment->size = cur->size - size * MEM_BLOCK_SIZE;
            cur->size = size * MEM_BLOCK_SIZE;
        }
        return (void*)((size_t)cur + (size_t)sizeof(FreeMem));
    }
    return 0; // memorija nije alocirana
}

void MemoryAllocator::compact(FreeMem* cur){
    if (!cur) return;
    if (cur->next && (size_t)cur + cur->size == (size_t)(cur->next)){
        // spajanje cur i cur->next
        cur->size += cur->next->size;
        cur->next = cur->next->next;
    }
}

int MemoryAllocator::_mem_free(void* addr) {
    if(addr == 0) return -1;
    FreeMem* cur = 0;

    void* address = (void*)((size_t)addr - sizeof(MemoryAllocator::FreeMem)); // address je pocetna adresa zaglavlja, a addr pocetna adresa prethodno zauzete memorije

    // ako nema slobodnog prostora ili je slobodan prostor na ranijoj adresi u odnosu na listu slobodnih fragmenata
    if (!freeMemHead || (size_t)address < (size_t)freeMemHead) cur = 0;
    // nadji poslednji slobodni fragment koji se nalazi ispred (na nizoj adresi) trazene adrese
    else for (cur = freeMemHead; cur->next != 0 && (size_t)address > (size_t)(cur->next); cur = cur->next);

    FreeMem* freedMemory = (FreeMem*)address;
    freedMemory->size = ((FreeMem*)address)->size;

    if (cur) {
        freedMemory->next = cur->next; // ako je nadjen fragment, novi se ubacuje izmedju nadjenog i sledeceg
        cur->next = freedMemory;
    }
    else {
        freedMemory->next = freeMemHead; // ako je null (nema prostora ili je adresa pre liste) postaje novi head
        freeMemHead = freedMemory;
    }

    compact(freedMemory);
    compact(cur);
    return 0;
}