#pragma once
#include "ast.hpp"

struct OptStats {
    int folded = 0;       // constant sub-expressions computed at compile time
    int propagated = 0;   // variable reads replaced by a known constant
    int simplified = 0;   // algebraic identities removed (x*1, x+0, ...)
};

// Runs constant propagation, constant folding and algebraic simplification over the program.
OptStats optimize(Program& prog);
