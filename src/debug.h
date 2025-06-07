#ifndef CXXLOX_DEBUG_H
#define CXXLOX_DEBUG_H

#include "chunk.h"

class Debugger
{
public:
    static void disassembleChunk(const Chunk &chunk, const char *name);

private:
    static int disassembleInstruction(const Chunk &chunk, int offset);
    static int constantInstruction(const char *name, const Chunk &chunk, int offset);
};

static inline void disassembleChunk(const Chunk &chunk, const char *name) { Debugger::disassembleChunk(chunk, name); }
#endif
