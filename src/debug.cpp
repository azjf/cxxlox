#include <iostream>

#include "chunk.h"
#include "debug.h"

static int simpleInstruction(const char *name, int offset)
{
    // GCC 12.2.0 does not support std::format()
    printf("%s\n", name);
    return offset + 1;
}

int Disassembler::constantInstruction(const char *name, const Chunk &chunk, int offset)
{
    auto constant = chunk.code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk.constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

void Disassembler::disassembleChunk(const Chunk &chunk, const char *name)
{
    printf("== %s ==\n", name);
    for (unsigned offset = 0; offset < chunk.code.size();) { offset = disassembleInstruction(chunk, offset); }
}


int Disassembler::disassembleInstruction(const Chunk &chunk, int offset)
{
    printf("%04d", offset);
    if (offset > 0 && chunk.lines[offset] == chunk.lines[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk.lines[offset]);
    }

    uint8_t instruction = chunk.code[offset];
    switch (instruction) {
    case OP_ADD:
        return simpleInstruction("OP_ADD", offset);
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_DEFINE_GLOBAL:
        return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
    case OP_DIVIDE:
        return simpleInstruction("OP_DIVIDE", offset);
    case OP_EQUAL:
        return simpleInstruction("OP_EQUAL", offset);
    case OP_FALSE:
        return simpleInstruction("OP_FALSE", offset);
    case OP_GET_GLOBAL:
        return constantInstruction("OP_GET_GLOBAL", chunk, offset);
    case OP_GREATER:
        return simpleInstruction("OP_GREATER", offset);
    case OP_LESS:
        return simpleInstruction("OP_LESS", offset);
    case OP_MULTIPLY:
        return simpleInstruction("OP_MULTIPLY", offset);
    case OP_NEGATE:
        return simpleInstruction("OP_NEGATE", offset);
    case OP_NOT:
        return simpleInstruction("OP_NOT", offset);
    case OP_NIL:
        return simpleInstruction("OP_NIL", offset);
    case OP_PRINT:
        return simpleInstruction("OP_PRINT", offset);
    case OP_POP:
        return simpleInstruction("OP_POP", offset);
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
    case OP_SET_GLOBAL:
        return constantInstruction("OP_SET_GLOBAL", chunk, offset);
    case OP_SUBTRACT:
        return simpleInstruction("OP_SUBTRACT", offset);
    case OP_TRUE:
        return simpleInstruction("OP_TRUE", offset);
    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
    }
    return 0;
}
