#ifndef CXXLOX_OBJECT_H
#define CXXLOX_OBJECT_H

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->getType())

#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString *) AS_OBJ(value))
#define AS_CSTRING(value) (static_cast<ObjString *>(AS_OBJ(value))->chars)

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
public:
    ObjString(char *chars, int length) : Obj(OBJ_STRING), length(length), chars(chars) {}
    ~ObjString();

public:
    int length;
    char *chars;
};

static inline bool isObjType(Value value, ObjType type) { return IS_OBJ(value) && AS_OBJ(value)->getType() == type; }

static inline ObjString *takeString(char *chars, int length) { return new ObjString(chars, length); }
ObjString *copyString(const char *chars, int length);
#endif
