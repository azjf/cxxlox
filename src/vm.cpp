#include <cstdarg>
#include <cstdio>

#include "chunk.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "value.h"
#include "vm.h"

static inline bool isFalsey(Value value) { return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value)); }

InterpretResult VM::interpret(const std::string &source)
{
    Chunk chunk;
    Compiler compiler;
    if (!compiler.compile(source, chunk)) { return INTERPRET_COMPILE_ERROR; }

    this->chunk = &chunk;
    ip = chunk.code.cbegin();

    InterpretResult result = run();
    return result;
}

InterpretResult VM::run()
{
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (chunk->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op)                          \
    do {                                                  \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtimeError("Operands must be numbers.");    \
            return INTERPRET_RUNTIME_ERROR;               \
        }                                                 \
        double b = AS_NUMBER(pop());                      \
        double a = AS_NUMBER(pop());                      \
        push(valueType(a op b));                          \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (const Value *slot = stack; slot != stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(*chunk, ip - chunk->code.cbegin());
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
        case OP_ADD: {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                concatenate();
            } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                double b = AS_NUMBER(pop());
                double a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            } else {
                runtimeError("Operand must be two numbers or two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_CONSTANT: {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_DEFINE_GLOBAL: {
            ObjString *name = READ_STRING();
            globals.set(name, peek(0));
            pop();
            break;
        }
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_EQUAL: {
            Value b = pop();
            Value a = pop();
            push(BOOL_VAL(valuesEqual(a, b)));
            break;
        }
        case OP_GET_GLOBAL: {
            ObjString *name = READ_STRING();
            Value value;
            if (!globals.get(name, &value)) {
                runtimeError("Undefined variable '%s'.", name->getChars());
                return INTERPRET_RUNTIME_ERROR;
            }
            push(value);
            break;
        }
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_NEGATE:
            if (!IS_NUMBER(peek(0))) {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
        case OP_PRINT: {
            printValue(pop());
            printf("\n");
            break;
        }
        case OP_POP:
            pop();
            break;
        case OP_RETURN: {
            return INTERPRET_OK;
        case OP_SET_GLOBAL: {
            ObjString *name = READ_STRING();
            if (globals.set(name, peek(0))) {
                globals.delete_(name);
                runtimeError("Undefined varible: '%s'", name->getChars());
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        }
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
}

void VM::runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = ip - chunk->code.cbegin() - 1;
    int line = chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);

    resetStack();
}

void VM::concatenate()
{
    ObjString *b = AS_STRING(pop());
    ObjString *a = AS_STRING(pop());
    ObjString *result = a->concatenate(*b);
    push(OBJ_VAL(result));
}

void VM::freeObjects()
{
    Obj *object = objects;
    while (object != NULL) {
        Obj *next = object->getNext();
        delete object;
        object = next;
    }
}
