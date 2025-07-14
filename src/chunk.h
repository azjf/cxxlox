#ifndef CXXLOX_CHUNK_H
#define CXXLOX_CHUNK_H

#include <vector>

#include "common.h"
#include "value.h"

enum OpCode
{
    OP_ADD,
    OP_CONSTANT,
    OP_DEFINE_GLOBAL,
    OP_DIVIDE,
    OP_EQUAL,
    OP_FALSE,
    OP_GET_GLOBAL,
    OP_GREATER,
    OP_LESS,
    OP_MULTIPLY,
    OP_NIL,
    OP_NOT,
    OP_NEGATE,
    OP_POP,
    OP_PRINT,
    OP_RETURN,
    OP_SET_GLOBAL,
    OP_SUBTRACT,
    OP_TRUE,
};

class Chunk
{
    friend class Disassembler;
    friend class VM;

public:
    void write(uint8_t byte, unsigned line)
    {
        code.push_back(byte);
        lines.push_back(line);
    }

    int addConstant(Value value)
    {
        constants.write(value);
        return constants.count() - 1;
    }

private:
    std::vector<uint8_t> code;
    std::vector<unsigned> lines;
    ValueArray constants;

    std::vector<uint8_t>::size_type count() { return code.size(); }
};

#endif
