#ifndef CXXLOX_DEBUG_H
#define CXXLOX_DEBUG_H

#include "chunk.h"

class Disassembler
{
public:
    static void disassembleChunk(const Chunk &chunk, const char *name);
    static int disassembleInstruction(const Chunk &chunk, int offset);

private:
    static int constantInstruction(const char *name, const Chunk &chunk, int offset);
};

static inline void disassembleChunk(const Chunk &chunk, const char *name) { Disassembler::disassembleChunk(chunk, name); }
static inline int disassembleInstruction(const Chunk &chunk, int offset) { return Disassembler::disassembleInstruction(chunk, offset); }
void printValue(Value value);
#endif
