#ifndef CXXLOX_VALUE_H
#define CXXLOX_VALUE_H

#include <vector>

#include "common.h"

typedef double Value;

class ValueArray
{
    friend class Chunk;
    friend class Disassembler;
    friend class VM;

public:
    void write(Value value) { values.push_back(value); }
    auto count() { return values.size(); }

private:
    std::vector<Value> values;
};

#endif
