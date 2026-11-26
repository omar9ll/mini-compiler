#include "codegen.hpp"
#include "lexer.hpp"
#include "optimizer.hpp"
#include "parser.hpp"
#include "vm.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static void usage() {
    std::cerr << "usage: minic [--no-opt] [--ast] [--dump] [--stats] <file.mc>\n";
}

int main(int argc, char** argv) {
    bool opt = true, showAst = false, dump = false, stats = false;
    std::string path;
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--no-opt") opt = false;
        else if (a == "--ast") showAst = true;
        else if (a == "--dump") dump = true;
        else if (a == "--stats") stats = true;
        else if (a[0] == '-') { usage(); return 2; }
        else path = a;
    }
    if (path.empty()) { usage(); return 2; }

    std::ifstream in(path);
    if (!in) { std::cerr << "minic: cannot open " << path << "\n"; return 2; }
    std::stringstream ss;
    ss << in.rdbuf();

    try {
        Program prog = Parser(Lexer(ss.str()).tokenize()).parseProgram();
        OptStats st;
        if (opt) st = optimize(prog);
        if (showAst)
            for (const auto& s : prog)
                std::cout << (s.kind == Stmt::Kind::Let ? "let " + s.name + " = " : std::string("print "))
                          << to_string(*s.expr) << "\n";
        Chunk chunk = generate(prog);
        if (dump) std::cout << disassemble(chunk);
        if (showAst || dump) return 0;
        long n = run(chunk, std::cout);
        if (stats)
            std::cerr << "[stats] instructions emitted: " << chunk.code.size() << ", executed: " << n
                      << ", folded: " << st.folded << ", propagated: " << st.propagated
                      << ", simplified: " << st.simplified << "\n";
    } catch (const CompileError& e) {
        std::cerr << "compile error: " << e.what() << "\n";
        return 1;
    } catch (const RuntimeError& e) {
        std::cerr << "runtime error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
