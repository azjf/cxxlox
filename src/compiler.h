#ifndef CXXLOX_COMPILER_H
#define CXXLOX_COMPILER_H

#include <memory>
#include <string>
#include <type_traits>

#include "scanner.h"
#include "value.h"
#include "vm.h"

struct Parser
{
    Token current;
    Token previous;
    bool hadError;
    bool panicMode;
};

enum Precedence
{
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
};

class Compiler;
using ParseFn = void (Compiler::*)(bool canAssign);

struct ParseRule
{
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
};

class Compiler
{
    friend struct ParseRule;

public:
    bool compile(const std::string &source, Chunk &chunk);

private:
    Parser parser;
    Chunk *compileChunk;
    std::unique_ptr<Scanner> scanner;
    static ParseRule rules[];

    static ParseRule &getRule(TokenType type) { return rules[type]; }

    void synchronize();

    void declaration();

    void varDeclaration();

    void statement();

    void printStatement();

    void expressionStatement();

    void expression() { parsePrecedence(PREC_ASSIGNMENT); }

    void number(bool canAssign)
    {
        double value = strtod(parser.previous.start, NULL);
        emitConstant(NUMBER_VAL(value));
    }

    void string(bool canAssign) { emitConstant(OBJ_VAL(copyString(parser.previous.start + 1, parser.previous.length - 2))); }

    void grouping(bool canAssign)
    {
        expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
    }

    void unary(bool canAssign);

    void binary(bool canAssign);

    void literal(bool canAssign);

    void errorAtCurrent(const char *message) { errorAt(parser.current, message); }

    void error(const char *message) { errorAt(parser.previous, message); }

    void errorAt(const Token &token, const char *message);

    void advance();

    bool match(TokenType type);

    bool check(TokenType type) { return parser.current.type == type; }

    void consume(TokenType type, const char *message);

    Chunk *currentChunk() { return compileChunk; }

    void emitByte(uint8_t byte) { currentChunk()->write(byte, parser.previous.line); }

    void endCompiler();

    void emitReturn() { emitByte(OP_RETURN); }

    void emitBytes(uint8_t byte1, uint8_t byte2)
    {
        emitByte(byte1);
        emitByte(byte2);
    }

    void emitConstant(Value value) { emitBytes(OP_CONSTANT, makeConstant(value)); }

    uint8_t makeConstant(Value value);

    void parsePrecedence(Precedence precedence);

    uint8_t identifierConstant(Token &name) { return makeConstant(OBJ_VAL(copyString(name.start, name.length))); }

    uint8_t parseVariable(const char *errorMessage)
    {
        consume(TOKEN_IDENTIFIER, errorMessage);
        return identifierConstant(parser.previous);
    }

    void defineVariable(uint8_t global) { emitBytes(OP_DEFINE_GLOBAL, global); }

    void variable(bool canAssign) { namedVariable(parser.previous, canAssign); }

    void namedVariable(Token name, bool canAssign);
};
#endif
