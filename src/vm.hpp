#pragma once
#include "bytecode.hpp"
#include <ostream>
#include <stdexcept>

struct RuntimeError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

// Executes bytecode on a value stack. Returns the number of instructions executed.
long run(const Chunk& c, std::ostream& out);
