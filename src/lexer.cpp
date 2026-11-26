#include "lexer.hpp"
#include <cctype>

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> out;
    while (pos_ < src_.size()) {
        char c = src_[pos_];
        if (c == '\n') { line_++; pos_++; continue; }
        if (std::isspace(static_cast<unsigned char>(c))) { pos_++; continue; }
        if (c == '#') {                                   // comment to end of line
            while (pos_ < src_.size() && src_[pos_] != '\n') pos_++;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            size_t start = pos_;
            while (pos_ < src_.size() &&
                   (std::isdigit(static_cast<unsigned char>(src_[pos_])) || src_[pos_] == '.'))
                pos_++;
            std::string num = src_.substr(start, pos_ - start);
            out.push_back({Tok::Number, num, std::stod(num), line_});
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            size_t start = pos_;
            while (pos_ < src_.size() &&
                   (std::isalnum(static_cast<unsigned char>(src_[pos_])) || src_[pos_] == '_'))
                pos_++;
            std::string word = src_.substr(start, pos_ - start);
            Tok kind = word == "let" ? Tok::Let : word == "print" ? Tok::Print : Tok::Ident;
            out.push_back({kind, word, 0, line_});
            continue;
        }
        Tok kind;
        switch (c) {
            case '+': kind = Tok::Plus; break;
            case '-': kind = Tok::Minus; break;
            case '*': kind = Tok::Star; break;
            case '/': kind = Tok::Slash; break;
            case '%': kind = Tok::Percent; break;
            case '(': kind = Tok::LParen; break;
            case ')': kind = Tok::RParen; break;
            case '=': kind = Tok::Assign; break;
            case ';': kind = Tok::Semi; break;
            default:
                throw CompileError("line " + std::to_string(line_) + ": unexpected character '" + c + "'");
        }
        out.push_back({kind, std::string(1, c), 0, line_});
        pos_++;
    }
    out.push_back({Tok::End, "<eof>", 0, line_});
    return out;
}
