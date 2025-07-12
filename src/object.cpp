#include <cstring>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

extern VM vm;

void *Obj::operator new(size_t size) { return reallocate(nullptr, 0, size); }

void Obj::operator delete(void *p) { reallocate(p, -1, 0); }

Obj::Obj(ObjType type)
{
    this->type = type;
    next = vm.objects;
    vm.objects = this;
}

ObjString::~ObjString() { FREE_ARRAY(char, chars, length + 1); }

ObjString *copyString(const char *chars, int length)
{
    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return new ObjString(heapChars, length);
}
