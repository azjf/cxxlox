#include <cstdint>

#include "chunk.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

bool Compiler::compile(const std::string &source, Chunk &chunk)
{
    scanner = std::make_unique<Scanner>(source);
    compileChunk = &chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    expression();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
    return !parser.hadError;
}

void Compiler::advance()
{
    parser.previous = parser.current;
    for (;;) {
        parser.current = scanner->scanToken();
        if (parser.current.type != TOKEN_ERROR) { break; }
        errorAtCurrent(parser.current.start);
    }
}

void Compiler::errorAt(const Token &token, const char *message)
{
    if (parser.panicMode) return;
    parser.panicMode = true;

    fprintf(stderr, "[line %d] Error", token.line);

    if (token.type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token.type == TOKEN_ERROR) {
        // Nothing.
    } else {
        fprintf(stderr, " at '%.*s'", token.length, token.start);
    }

    fprintf(stderr, ": %s\n", message);
    parser.hadError = true;
}

void Compiler::consume(TokenType type, const char *message)
{
    if (parser.current.type == type) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

uint8_t Compiler::makeConstant(Value value)
{
    int constant = currentChunk()->addConstant(value);
    if (constant > UINT8_MAX) {
        error("Too many constants in one chunk.");
        return 0;
    }
    return static_cast<uint8_t>(constant);
}

void Compiler::unary()
{
    TokenType operatorType = parser.previous.type;
    parsePrecedence(PREC_UNARY);
    switch (operatorType) {
    case TOKEN_BANG:
        emitByte(OP_NOT);
        break;
    case TOKEN_MINUS:
        emitByte(OP_NEGATE);
        break;
    default:
        return;
    }
}

ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {&Compiler::grouping, nullptr, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_COMMA] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_DOT] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_MINUS] = {&Compiler::unary, &Compiler::binary, PREC_TERM},
    [TOKEN_PLUS] = {nullptr, &Compiler::binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_SLASH] = {nullptr, &Compiler::binary, PREC_FACTOR},
    [TOKEN_STAR] = {nullptr, &Compiler::binary, PREC_FACTOR},
    [TOKEN_BANG] = {&Compiler::unary, nullptr, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {nullptr, &Compiler::binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {nullptr, &Compiler::binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {nullptr, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {nullptr, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_LESS] = {nullptr, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {nullptr, &Compiler::binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_STRING] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_NUMBER] = {&Compiler::number, nullptr, PREC_NONE},
    [TOKEN_AND] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_CLASS] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_ELSE] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_FALSE] = {&Compiler::literal, nullptr, PREC_NONE},
    [TOKEN_FOR] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_FUN] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_IF] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_NIL] = {&Compiler::literal, nullptr, PREC_NONE},
    [TOKEN_OR] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_PRINT] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_RETURN] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_SUPER] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_THIS] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_TRUE] = {&Compiler::literal, nullptr, PREC_NONE},
    [TOKEN_VAR] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_WHILE] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_ERROR] = {nullptr, nullptr, PREC_NONE},
    [TOKEN_EOF] = {nullptr, nullptr, PREC_NONE},
};

ParseRule &getRule(TokenType type) { return rules[type]; }

void Compiler::binary()
{
    TokenType operatorType = parser.previous.type;
    ParseRule &rule = getRule(operatorType);
    parsePrecedence(static_cast<Precedence>(rule.precedence + 1));

    switch (operatorType) {
    case TOKEN_BANG_EQUAL:
        emitBytes(OP_EQUAL, OP_NOT);
        break;
    case TOKEN_EQUAL_EQUAL:
        emitByte(OP_EQUAL);
        break;
    case TOKEN_GREATER:
        emitByte(OP_GREATER);
        break;
    case TOKEN_GREATER_EQUAL:
        emitBytes(OP_LESS, OP_NOT);
        break;
    case TOKEN_LESS:
        emitByte(OP_LESS);
        break;
    case TOKEN_LESS_EQUAL:
        emitBytes(OP_GREATER, OP_NOT);
        break;
    case TOKEN_PLUS:
        emitByte(OP_ADD);
        break;
    case TOKEN_MINUS:
        emitByte(OP_SUBTRACT);
        break;
    case TOKEN_STAR:
        emitByte(OP_MULTIPLY);
        break;
    case TOKEN_SLASH:
        emitByte(OP_DIVIDE);
        break;
    default:
        return;
    }
}

void Compiler::parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.previous.type).prefix;
    if (prefixRule == nullptr) {
        error("Expect expression.");
        return;
    }
    (this->*prefixRule)();

    while (precedence <= getRule(parser.current.type).precedence) {
        advance();
        ParseFn infixRule = getRule(parser.previous.type).infix;
        (this->*infixRule)();
    }
}

void Compiler::endCompiler()
{
    emitReturn();
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError) { disassembleChunk(*currentChunk(), "code"); }
#endif
}


void Compiler::literal()
{
    switch (parser.previous.type) {
    case TOKEN_FALSE:
        emitByte(OP_FALSE);
        break;
    case TOKEN_NIL:
        emitByte(OP_NIL);
        break;
    case TOKEN_TRUE:
        emitByte(OP_TRUE);
        break;
    default:
        return;
    }
}
