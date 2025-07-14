#ifndef CXXLOX_VM_H
#define CXXLOX_VM_H

#include <memory>
#include <stack>

#include "chunk.h"
#include "memory.h"
#include "object.h"
#include "table.h"

constexpr unsigned STACK_MAX = 256;

enum InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

class VM
{
    friend class Obj;
    friend class ObjString;
    friend ObjString *copyString(const char *chars, int length);
    friend ObjString *takeString(char *chars, int length);

public:
    ~VM() { freeObjects(); }

    InterpretResult interpret(const std::string &source);

    void resetStack() { stackTop = stack; }
    void push(Value value) { *stackTop++ = value; }
    Value pop() { return *--stackTop; }
    Value peek(int distance) { return stackTop[-1 - distance]; }

private:
    const Chunk *chunk;
    std::vector<uint8_t>::const_iterator ip;
    Table globals;
    Table strings;
    Obj *objects = nullptr;

    // std::stack can not be used here, because we need to iterate through it later
    Value stack[STACK_MAX];
    Value *stackTop = stack;

    InterpretResult run();

    void runtimeError(const char *format, ...);

    void concatenate();

    void freeObjects();
};
#endif
