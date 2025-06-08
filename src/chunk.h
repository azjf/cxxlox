#ifndef CXXLOX_CHUNK_H
#define CXXLOX_CHUNK_H

#include <vector>

#include "common.h"
#include "value.h"

enum OpCode
{
    OP_CONSTANT,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NEGATE,
    OP_RETURN,
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
