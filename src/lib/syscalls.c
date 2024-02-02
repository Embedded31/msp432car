#include <errno.h>
#include <sys/stat.h>

int _read(int file, char *ptr, int len) {
    // Implement your own _read() if needed
    return 0;
}

int _write(int file, char *ptr, int len) {
    // Implement your own _write() if needed
    return len;
}

caddr_t _sbrk(int incr) {
    // Implement your own _sbrk() if needed
    extern char __heap_start__; // Defined by the linker
    extern char __heap_end__;   // Defined by the linker

    static char *heap_end = &__heap_start__;

    char *prev_heap_end = heap_end;

    if (heap_end + incr > &__heap_end__) {
        // Heap overflow, return error
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

int _close(int file) {
    // Implement your own _close() if needed
    return -1;
}

int _fstat(int file, struct stat *st) {
    // Implement your own _fstat() if needed
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    // Implement your own _isatty() if needed
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    // Implement your own _lseek() if needed
    return 0;
}
