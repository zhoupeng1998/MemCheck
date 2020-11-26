#ifndef MEM_CHECK_HPP
#define MEM_CHECK_HPP

void* operator new (size_t _size, char* _file, unsigned int _line);
void* operator new[] (size_t _size, char* _file, unsigned int _line);

#ifndef __NEW_OVERLOAD_IMPLEMENTATION__
#define new new(__FILE__, __LINE__)
#endif

namespace MemCheck {
    class MallocCounter {
        static unsigned int printLeak() noexcept;
    public:
        static unsigned int mallocCount;

        MallocCounter() noexcept {
            ++mallocCount;
        }

        ~MallocCounter() noexcept {
            if (--mallocCount == 0) {
                printLeak();
            }
        }
    };

    static MallocCounter _exit_counter;
}




#endif
