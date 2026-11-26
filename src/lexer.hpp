#pragma once
#include <string>
#include <vector>
#include <stdexcept>

enum class Tok { Number, Ident, Let, Print, Plus, Minus, Star, Slash, Percent,
                 LParen, RParen, Assign, Semi, End };

struct Token {
    Tok kind;
    std::string text;
    double value = 0;
    int line = 1;
};

struct CompileError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// Turns source text into a flat list of tokens.
class Lexer {
public:
    explicit Lexer(std::string src) : src_(std::move(src)) {}
    std::vector<Token> tokenize();

private:
    std::string src_;
    size_t pos_ = 0;
    int line_ = 1;
};
