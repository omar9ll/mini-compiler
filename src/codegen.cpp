#include "codegen.hpp"
#include "lexer.hpp"
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace {

class Gen {
public:
    Chunk chunk;

    void stmt(const Stmt& s) {
        expr(*s.expr);
        if (s.kind == Stmt::Kind::Let) emit(Op::Store, slotFor(s.name), s.expr->line);
        else emit(Op::Print, 0, s.expr->line);
    }

private:
    std::unordered_map<std::string, int> slots_;
    std::unordered_map<double, int> constIndex_;

    void emit(Op op, int arg, int line) { chunk.code.push_back({op, arg, line}); }

    int slotFor(const std::string& name) {
        auto it = slots_.find(name);
        if (it != slots_.end()) return it->second;
        int idx = static_cast<int>(chunk.slotNames.size());
        slots_[name] = idx;
        chunk.slotNames.push_back(name);
        return idx;
    }

    int constant(double v) {
        auto it = constIndex_.find(v);
        if (it != constIndex_.end()) return it->second;   // reuse identical constants
        int idx = static_cast<int>(chunk.constants.size());
        chunk.constants.push_back(v);
        constIndex_[v] = idx;
        return idx;
    }

    void expr(const Expr& e) {
        switch (e.kind) {
            case Expr::Kind::Number: emit(Op::Const, constant(e.value), e.line); break;
            case Expr::Kind::Var: {
                auto it = slots_.find(e.name);
                if (it == slots_.end())
                    throw CompileError("line " + std::to_string(e.line) + ": variable '" + e.name +
                                       "' used before it was defined");
                emit(Op::Load, it->second, e.line);
                break;
            }
            case Expr::Kind::Unary: expr(*e.lhs); emit(Op::Neg, 0, e.line); break;
            case Expr::Kind::Binary: {
                expr(*e.lhs);
                expr(*e.rhs);
                Op op = e.op == '+' ? Op::Add : e.op == '-' ? Op::Sub : e.op == '*' ? Op::Mul
                      : e.op == '/' ? Op::Div : Op::Mod;
                emit(op, 0, e.line);
                break;
            }
        }
    }
};

const char* opName(Op op) {
    switch (op) {
        case Op::Const: return "CONST"; case Op::Load: return "LOAD"; case Op::Store: return "STORE";
        case Op::Neg: return "NEG"; case Op::Add: return "ADD"; case Op::Sub: return "SUB";
        case Op::Mul: return "MUL"; case Op::Div: return "DIV"; case Op::Mod: return "MOD";
        case Op::Print: return "PRINT"; case Op::Halt: return "HALT";
    }
    return "?";
}

}  // namespace

Chunk generate(const Program& prog) {
    Gen g;
    for (const auto& s : prog) g.stmt(s);
    g.chunk.code.push_back({Op::Halt, 0, 0});
    return g.chunk;
}

std::string disassemble(const Chunk& c) {
    std::ostringstream os;
    for (size_t i = 0; i < c.code.size(); i++) {
        const Instr& in = c.code[i];
        os << std::setw(4) << std::setfill('0') << i << "  " << std::left << std::setw(6)
           << std::setfill(' ') << opName(in.op) << std::right;
        if (in.op == Op::Const) os << " " << c.constants[in.arg];
        if (in.op == Op::Load || in.op == Op::Store) os << " " << c.slotNames[in.arg];
        os << "\n";
    }
    return os.str();
}
