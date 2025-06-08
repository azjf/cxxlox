#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[])
{
    VM vm;
    Chunk chunk;

    int constant = chunk.addConstant(1.2);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    constant = chunk.addConstant(3.4);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_ADD, 123);

    constant = chunk.addConstant(5.6);
    chunk.write(OP_CONSTANT, 123);
    chunk.write(constant, 123);

    chunk.write(OP_DIVIDE, 123);
    chunk.write(OP_NEGATE, 123);

    chunk.write(OP_RETURN, 123);

    disassembleChunk(chunk, "test chunk");
    vm.interpret(chunk);
    return 0;
}
