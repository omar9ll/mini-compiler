#pragma once
#include "ast.hpp"
#include "lexer.hpp"

// Recursive-descent parser.
//   program   := stmt*
//   stmt      := 'let' IDENT '=' expr ';' | 'print' expr ';'
//   expr      := term (('+' | '-') term)*
//   term      := unary (('*' | '/' | '%') unary)*
//   unary     := '-' unary | primary
//   primary   := NUMBER | IDENT | '(' expr ')'
class Parser {
public:
    explicit Parser(std::vector<Token> toks) : toks_(std::move(toks)) {}
    Program parseProgram();

private:
    std::vector<Token> toks_;
    size_t pos_ = 0;

    const Token& peek() const { return toks_[pos_]; }
    const Token& advance() { return toks_[pos_++]; }
    bool match(Tok k) { if (peek().kind == k) { pos_++; return true; } return false; }
    const Token& expect(Tok k, const char* what);

    Stmt parseStmt();
    std::unique_ptr<Expr> parseExpr();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseUnary();
    std::unique_ptr<Expr> parsePrimary();
};
