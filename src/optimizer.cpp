#include "optimizer.hpp"
#include <cmath>
#include <optional>
#include <unordered_map>

namespace {

using Env = std::unordered_map<std::string, double>;  // variables whose value is known at compile time

bool isNum(const Expr& e, double v) { return e.kind == Expr::Kind::Number && e.value == v; }

std::optional<double> evalBinary(char op, double a, double b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b == 0) return std::nullopt; return a / b;           // leave div-by-zero for runtime
        case '%': if (b == 0) return std::nullopt; return std::fmod(a, b);
    }
    return std::nullopt;
}

void fold(std::unique_ptr<Expr>& e, const Env& env, OptStats& st) {
    switch (e->kind) {
        case Expr::Kind::Number:
            return;
        case Expr::Kind::Var: {
            auto it = env.find(e->name);
            if (it != env.end()) { e = Expr::number(it->second, e->line); st.propagated++; }
            return;
        }
        case Expr::Kind::Unary:
            fold(e->lhs, env, st);
            if (e->lhs->kind == Expr::Kind::Number) { e = Expr::number(-e->lhs->value, e->line); st.folded++; }
            return;
        case Expr::Kind::Binary: {
            fold(e->lhs, env, st);
            fold(e->rhs, env, st);
            if (e->lhs->kind == Expr::Kind::Number && e->rhs->kind == Expr::Kind::Number) {
                if (auto v = evalBinary(e->op, e->lhs->value, e->rhs->value)) {
                    e = Expr::number(*v, e->line); st.folded++;
                }
                return;
            }
            // Algebraic identities: x+0, 0+x, x-0, x*1, 1*x, x/1  ->  x
            char op = e->op;
            if ((op == '+' && isNum(*e->rhs, 0)) || (op == '-' && isNum(*e->rhs, 0)) ||
                (op == '*' && isNum(*e->rhs, 1)) || (op == '/' && isNum(*e->rhs, 1))) {
                e = std::move(e->lhs); st.simplified++; return;
            }
            if ((op == '+' && isNum(*e->lhs, 0)) || (op == '*' && isNum(*e->lhs, 1))) {
                e = std::move(e->rhs); st.simplified++; return;
            }
            return;
        }
    }
}

}  // namespace

OptStats optimize(Program& prog) {
    OptStats st;
    Env env;
    for (auto& s : prog) {
        fold(s.expr, env, st);
        if (s.kind == Stmt::Kind::Let) {
            if (s.expr->kind == Expr::Kind::Number) env[s.name] = s.expr->value;
            else env.erase(s.name);   // value no longer known at compile time
        }
    }
    return st;
}
