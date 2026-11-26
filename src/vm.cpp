#include "vm.hpp"
#include <cmath>
#include <vector>

long run(const Chunk& c, std::ostream& out) {
    std::vector<double> stack;
    std::vector<double> slots(c.slotNames.size(), 0.0);
    long executed = 0;
    auto pop = [&] { double v = stack.back(); stack.pop_back(); return v; };

    for (size_t ip = 0; ip < c.code.size(); ip++) {
        const Instr& in = c.code[ip];
        executed++;
        switch (in.op) {
            case Op::Const: stack.push_back(c.constants[in.arg]); break;
            case Op::Load:  stack.push_back(slots[in.arg]); break;
            case Op::Store: slots[in.arg] = pop(); break;
            case Op::Neg:   stack.back() = -stack.back(); break;
            case Op::Add: { double b = pop(); stack.back() += b; break; }
            case Op::Sub: { double b = pop(); stack.back() -= b; break; }
            case Op::Mul: { double b = pop(); stack.back() *= b; break; }
            case Op::Div:
            case Op::Mod: {
                double b = pop();
                if (b == 0) throw RuntimeError("line " + std::to_string(in.line) + ": division by zero");
                stack.back() = in.op == Op::Div ? stack.back() / b : std::fmod(stack.back(), b);
                break;
            }
            case Op::Print: out << pop() << "\n"; break;
            case Op::Halt: return executed;
        }
    }
    return executed;
}
