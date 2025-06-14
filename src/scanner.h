#ifndef CXXLOX_SCANNER_H
#define CXXLOX_SCANNER_H

#include <cstring>
#include <string>

enum TokenType
{
    // Single-character tokens.
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_SLASH,
    TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FUN,
    TOKEN_IF,
    TOKEN_NIL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_SUPER,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,
    TOKEN_ERROR,
    TOKEN_EOF
};

struct Token
{
    TokenType type;
    const char *start;
    int length;
    int line;

    Token() = default;

    Token(TokenType type, const char *start, int length, int line) : type(type), start(start), length(length), line(line) {}
};

class Scanner
{
    friend struct Token;

public:
    Scanner(const std::string &source) : source(source.c_str()), start(this->source), current(this->source) {}

    Token scanToken();

private:
    const char *source;
    const char *start;
    const char *current;
    unsigned line = 1;

    bool isAtEnd() { return *current == '\0'; }

    Token makeToken(TokenType type) { return Token(type, start, static_cast<int>(current - start), line); }


    Token errorToken(const char *message) { return Token(TOKEN_ERROR, message, static_cast<int>(strlen(message)), line); }

    char advance() { return *current++; }

    char peek() { return *current; }

    char peekNext() { return isAtEnd() ? '\0' : current[1]; }

    bool match(char expected);

    void skipWhitespace();

    Token string();
    Token number();
    Token identifier();

    TokenType identiferType();
    TokenType checkKeyword(int start, int length, const char *rest, TokenType type);
};

#endif
