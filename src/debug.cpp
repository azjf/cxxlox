#include <iostream>

#include "chunk.h"
#include "debug.h"

static int simpleInstruction(const char *name, int offset)
{
    // GCC 12.2.0 does not support std::format()
    printf("%s\n", name);
    return offset + 1;
}

static void printValue(Value value) { printf("%g", value); }

int Debugger::constantInstruction(const char *name, const Chunk &chunk, int offset)
{
    auto constant = chunk.code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk.constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

void Debugger::disassembleChunk(const Chunk &chunk, const char *name)
{
    printf("== %s ==\n", name);
    for (unsigned offset = 0; offset < chunk.code.size();) { offset = disassembleInstruction(chunk, offset); }
}


int Debugger::disassembleInstruction(const Chunk &chunk, int offset)
{
    printf("%04d ", offset);
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk.lines[offset]);
    }

    uint8_t instruction = chunk.code[offset];
    switch (instruction) {
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
    return 0;
}
