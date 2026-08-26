#include "Lexer.h"

bool Lexer::isSign(char ch) {
    char signs[] = "=+-*/.`~!@#$%^&*()_{}[];:'\"\\|<>,?/";
    for (std::size_t i = 0;  i < sizeof(signs) - 1;  i ++) {
        if (ch == signs[i]) return true;
    }
    return false;
}

Token Lexer::signToken() {
    Token tk;
    char cur;
    char pre = this->file->current();
    tk.type = tokenTypeSign;
    tk.addToContent(pre);

    if (!this->file->next()) return tk;
    cur = this->file->current();
    if (!this->isSign(cur)) return tk;

    switch (this->isSignTwoChars(pre, cur)) {
        case signStateRight:
            tk.addToContent(cur);
            break;
        case signStateWrong:
            return tk;
        case signStateComment:
            return this->getNextToken();
    }

    this->file->next();
    return tk;
}

enum signState Lexer::isSignTwoChars(char a, char b) {
    switch (a) {
        case '+':
            switch (b) {
                case '+': return signStateRight;
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '-':
            switch (b) {
                case '-': return signStateRight;
                case '=': return signStateRight;
                case '>': return signStateRight;
                default: return signStateWrong;
            }
        case '*':
            switch (b) {
                case '*': return signStateRight;
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '/':
            switch (b) {
                case '=': return signStateRight;
                case '/': // comment
                    this->file->skipToNextLine();
                    return signStateComment;
                default: return signStateWrong;
            }
        case '^':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '&':
            switch (b) {
                case '&': return signStateRight;
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '%':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '!':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '=':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '>':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        case '<':
            switch (b) {
                case '=': return signStateRight;
                default: return signStateWrong;
            }
        default: return signStateWrong;
    }
}
