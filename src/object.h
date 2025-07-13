#ifndef CXXLOX_OBJECT_H
#define CXXLOX_OBJECT_H

#include <cstring>

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->getType())

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *) AS_OBJ(value))
#define AS_CSTRING(value) (static_cast<ObjString *>(AS_OBJ(value))->getChars())

enum ObjType
{
    OBJ_STRING,
};

class Obj
{
public:
    void *operator new(size_t size);
    void operator delete(void *p);

    Obj(ObjType type);
    virtual ~Obj() = default;

    bool getType() { return type; }
    Obj *getNext() { return next; }

private:
    ObjType type;
    Obj *next = nullptr;
};

class ObjString : public Obj
{
    friend bool operator==(const ObjString &lhs, const ObjString &rhs);
    friend struct std::hash<ObjString *>;

public:
    ObjString(char *chars, int length, uint32_t hash);
    ~ObjString();

    // the semantics of concatenate() is different from operator+()
    ObjString *concatenate(const ObjString &rhs);

    bool equals(const char *chars, int length, uint32_t hash) const
    {
        return this->length == length && this->hash == hash && !std::memcmp(this->chars, chars, length);
    }

    char *getChars() const { return chars; }

private:
    int length;
    char *chars;
    uint32_t hash;
};

inline bool operator==(const ObjString &lhs, const ObjString &rhs) { return lhs.equals(rhs.chars, rhs.length, rhs.hash); }

template <>
struct std::hash<ObjString *>
{
    size_t operator()(const ObjString *s) const noexcept { return s->hash; }
};

static inline bool isObjType(Value value, ObjType type) { return IS_OBJ(value) && AS_OBJ(value)->getType() == type; }

ObjString *takeString(char *chars, int length);
ObjString *copyString(const char *chars, int length);
#endif
