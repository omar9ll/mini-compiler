#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class Op : uint8_t {
    Const,   // push constants[arg]
    Load,    // push slots[arg]
    Store,   // slots[arg] = pop
    Neg, Add, Sub, Mul, Div, Mod,
    Print,   // print pop
    Halt
};

struct Instr {
    Op op;
    int32_t arg = 0;
    int line = 0;
};

struct Chunk {
    std::vector<Instr> code;
    std::vector<double> constants;
    std::vector<std::string> slotNames;  // slot index -> variable name (for disassembly)
};

std::string disassemble(const Chunk& c);
