#pragma once
#include "ast.hpp"
#include "bytecode.hpp"

// Walks the AST and emits stack-machine bytecode.
// Also checks that every variable is defined (with `let`) before it is used.
Chunk generate(const Program& prog);
