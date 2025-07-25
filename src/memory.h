#ifndef CXXLOX_MEMORY_H
#define CXXLOX_MEMORY_H

#include "common.h"
#include "object.h"

#define ALLOCATE(type, count) (type *) reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) *2)

#define FREE_ARRAY(type, pointer, oldCount) reallocate(pointer, sizeof(type) * (oldCount), 0)

#define GROW_ARRAY(type, pointer, oldCount, newCount) (type *) reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * newCount)

void *reallocate(void *pointer, size_t oldSize, size_t newSize);
#endif
