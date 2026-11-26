#include "ast.hpp"
#include <sstream>

std::unique_ptr<Expr> Expr::number(double v, int line) {
    auto e = std::make_unique<Expr>(); e->kind = Kind::Number; e->value = v; e->line = line; return e;
}
std::unique_ptr<Expr> Expr::var(std::string n, int line) {
    auto e = std::make_unique<Expr>(); e->kind = Kind::Var; e->name = std::move(n); e->line = line; return e;
}
std::unique_ptr<Expr> Expr::unary(char op, std::unique_ptr<Expr> x, int line) {
    auto e = std::make_unique<Expr>(); e->kind = Kind::Unary; e->op = op; e->lhs = std::move(x); e->line = line; return e;
}
std::unique_ptr<Expr> Expr::binary(char op, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r, int line) {
    auto e = std::make_unique<Expr>(); e->kind = Kind::Binary; e->op = op;
    e->lhs = std::move(l); e->rhs = std::move(r); e->line = line; return e;
}

std::string to_string(const Expr& e) {
    std::ostringstream os;
    switch (e.kind) {
        case Expr::Kind::Number: os << e.value; break;
        case Expr::Kind::Var: os << e.name; break;
        case Expr::Kind::Unary: os << "(" << e.op << to_string(*e.lhs) << ")"; break;
        case Expr::Kind::Binary:
            os << "(" << to_string(*e.lhs) << " " << e.op << " " << to_string(*e.rhs) << ")"; break;
    }
    return os.str();
}
