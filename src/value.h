#ifndef CXXLOX_VALUE_H
#define CXXLOX_VALUE_H

#include <vector>

#include "common.h"

enum ValueType
{
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER
};

struct Value
{
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
};

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

class ValueArray
{
    friend class Chunk;
    friend class Disassembler;
    friend class VM;
    friend bool valuesEqual(Value a, Value b);

public:
    void write(Value value) { values.push_back(value); }
    auto count() { return values.size(); }

private:
    std::vector<Value> values;
};

bool valuesEqual(Value a, Value b);

#endif
