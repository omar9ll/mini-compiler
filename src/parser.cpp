#include "parser.hpp"

const Token& Parser::expect(Tok k, const char* what) {
    if (peek().kind != k)
        throw CompileError("line " + std::to_string(peek().line) + ": expected " + what +
                           " but found '" + peek().text + "'");
    return advance();
}

Program Parser::parseProgram() {
    Program prog;
    while (peek().kind != Tok::End) prog.push_back(parseStmt());
    return prog;
}

Stmt Parser::parseStmt() {
    Stmt s;
    if (match(Tok::Let)) {
        s.kind = Stmt::Kind::Let;
        s.name = expect(Tok::Ident, "a variable name").text;
        expect(Tok::Assign, "'='");
        s.expr = parseExpr();
    } else if (match(Tok::Print)) {
        s.kind = Stmt::Kind::Print;
        s.expr = parseExpr();
    } else {
        throw CompileError("line " + std::to_string(peek().line) + ": expected 'let' or 'print' but found '" +
                           peek().text + "'");
    }
    expect(Tok::Semi, "';'");
    return s;
}

std::unique_ptr<Expr> Parser::parseExpr() {
    auto lhs = parseTerm();
    while (peek().kind == Tok::Plus || peek().kind == Tok::Minus) {
        const Token& op = advance();
        lhs = Expr::binary(op.text[0], std::move(lhs), parseTerm(), op.line);
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    auto lhs = parseUnary();
    while (peek().kind == Tok::Star || peek().kind == Tok::Slash || peek().kind == Tok::Percent) {
        const Token& op = advance();
        lhs = Expr::binary(op.text[0], std::move(lhs), parseUnary(), op.line);
    }
    return lhs;
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (peek().kind == Tok::Minus) {
        int line = advance().line;
        return Expr::unary('-', parseUnary(), line);
    }
    return parsePrimary();
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    const Token& t = peek();
    if (match(Tok::Number)) return Expr::number(t.value, t.line);
    if (match(Tok::Ident)) return Expr::var(t.text, t.line);
    if (match(Tok::LParen)) {
        auto e = parseExpr();
        expect(Tok::RParen, "')'");
        return e;
    }
    throw CompileError("line " + std::to_string(t.line) + ": expected an expression but found '" + t.text + "'");
}
