#include <cstdlib>

#include "memory.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    if (newSize == 0) {
        free(pointer);
        return nullptr;
    }

    void *result = realloc(pointer, newSize);
    if (!result) { exit(1); }
    return result;
}
