#include <iostream>
#include <cstring>

#define __NEW_OVERLOAD_IMPLEMENTATION__
#include "MemCheck.hpp"

namespace MemCheck {
    struct MallocList {
        MallocList* next;
        MallocList* prev;
        size_t size;
        bool isArray;
        char* file;
        unsigned int line;
    };

    static unsigned long memory_allocated = 0;

    static MallocList root = {&root, &root, 0, false, nullptr, 0};

    void* allocateMemory (size_t _size, bool _array, char* _file, unsigned int _line) {
        size_t newSize = sizeof (MallocList) + _size;
        MallocList* newElem = (MallocList*)malloc(newSize);
        newElem->next = root.next;
        newElem->prev = &root;
        newElem->size = _size;
        newElem->isArray = _array;
        newElem->file = nullptr;
        if (_file) {
            newElem->file = (char*)malloc(strlen(_file) + 1);
            strcpy(newElem->file, _file);
        }
        newElem->line = _line;
        root.next->prev = newElem;
        root.next = newElem;
        memory_allocated += _size;
        return (char*)newElem + sizeof(MallocList);
    }

    void deallocateMemory (void* _ptr, bool _array) {
        MallocList* currentElem = (MallocList*)((char*)_ptr - sizeof(MallocList));
        if (currentElem->isArray != _array)
            return;
        currentElem->prev->next = currentElem->next;
        currentElem->next->prev = currentElem->prev;
        memory_allocated -= currentElem->size;
        if (currentElem->file) {
            free(currentElem->file);
        }
        free(currentElem);
    }
}

unsigned int MemCheck::MallocCounter::mallocCount = 0;

void* operator new (size_t _size) {
    return MemCheck::allocateMemory(_size, false, nullptr, 0);
}

void* operator new[] (size_t _size) {
    return MemCheck::allocateMemory(_size, true, nullptr, 0);
}

void* operator new (size_t _size, char* _file, unsigned int _line) {
    return MemCheck::allocateMemory(_size, false, _file, _line);
}

void* operator new[] (size_t _size, char* _file, unsigned int _line) {
    return MemCheck::allocateMemory(_size, true, _file, _line);
}

void operator delete (void* _ptr) noexcept {
    MemCheck::deallocateMemory(_ptr, false);
}

void operator delete[] (void* _ptr) noexcept {
    MemCheck::deallocateMemory(_ptr, true);
}

unsigned int MemCheck::MallocCounter::printLeak () noexcept {
    unsigned int count = 0;
    MallocList* ptr = root.next;
    while (ptr && ptr != &root) {
        if (ptr->isArray) {
            std::cout << "leak[] ";
        } else {
            std::cout << "leak ";
        }
        std::cout << ptr << " size " << ptr->size;
        if (ptr->file) {
            std::cout << " (Located in " << ptr->file << " line " << ptr->line << ")";
        } else {
            std::cout << " (Location not found)";
        }
        std::cout << std::endl;
        count++;
        ptr = ptr->next;
    }
    if (count) {
        std::cout << "Total " << count << " leaks, size " << memory_allocated << " bytes." << std::endl;
    }
    return count;
}
