#pragma once
#include <memory>
#include <string>
#include <vector>

// Expression tree.
struct Expr {
    enum class Kind { Number, Var, Unary, Binary } kind;
    double value = 0;          // Number
    std::string name;          // Var
    char op = 0;               // Unary ('-') / Binary ('+','-','*','/','%')
    std::unique_ptr<Expr> lhs, rhs;  // Unary uses lhs only
    int line = 1;

    static std::unique_ptr<Expr> number(double v, int line);
    static std::unique_ptr<Expr> var(std::string n, int line);
    static std::unique_ptr<Expr> unary(char op, std::unique_ptr<Expr> e, int line);
    static std::unique_ptr<Expr> binary(char op, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r, int line);
};

// A program is a list of statements: `let x = expr;` or `print expr;`.
struct Stmt {
    enum class Kind { Let, Print } kind;
    std::string name;              // Let target
    std::unique_ptr<Expr> expr;
};

using Program = std::vector<Stmt>;

std::string to_string(const Expr& e);  // prints the tree in fully parenthesized form
