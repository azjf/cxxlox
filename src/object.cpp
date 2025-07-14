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

ObjString::ObjString(char *chars, int length, uint32_t hash) : Obj(OBJ_STRING), length(length), chars(chars), hash(hash) { vm.strings.set(this, NIL_VAL); }

ObjString::~ObjString() { FREE_ARRAY(char, chars, length + 1); }

static uint32_t hashString(const char *key, int length)
{
    uint32_t hash = 2166136261u;
    for (int i = 0; i != length; ++i) {
        hash ^= key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString *takeString(char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = vm.strings.findString(chars, length, hash);
    if (interned != nullptr) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }
    return new ObjString(chars, length, hash);
}

ObjString *copyString(const char *chars, int length)
{
    uint32_t hash = hashString(chars, length);
    ObjString *interned = vm.strings.findString(chars, length, hash);
    if (interned != nullptr) { return interned; }
    char *heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return new ObjString(heapChars, length, hash);
}

ObjString *ObjString::concatenate(const ObjString &rhs)
{
    int length = this->length + rhs.length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, this->chars, this->length);
    memcpy(chars + this->length, rhs.chars, rhs.length);
    chars[length] = '\0';
    return takeString(chars, length);
}
