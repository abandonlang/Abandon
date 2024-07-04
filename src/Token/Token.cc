#include "../include.h"
bool Token::idToKeyword() {
    // fn, int, float, string
    switch (this->content.length()) {
        case 2:
            if("fn" == this->content) break;
            return false;
        case 3:
            if("int" == this->content) break;
            return false;
        case 5:
            if("float" == this->content) break;
            return false;
        case 6:
            if("string" == this->content) break;
            return false;
        default:
            return false;
    }
    this->type = tokenTypeKeyword;
    return true;
}
bool Token::matchSign(std::string content) {
    if (this->type == tokenTypeSign) {
        if (this->content == content) return true;
    }
    return false;
}
bool Token::matchKeyword(std::string content) {
    if (this->type == tokenTypeKeyword) {
        if (this->content == content) return true;
    }
    return true;
}
void Token::addToContent(char newChar) {
    char tmp[2] = {0};
    tmp[0] = newChar;
    this->content.append(tmp);
}
void Token::addToContent(int newChar) {
    this->addToContent((char)(newChar+'0'));
}
bool Token::isEof() {
    return this->type == tokenTypeEof;
}
#ifdef DEBUG
std::string Token::typeToText() {
    switch (this->type) {
        case tokenTypeEof:
            return "Eof";
        case tokenTypeChar:
            return "Char";
        case tokenTypeFloat:
            return "Float";
        case tokenTypeId:
            return "Id";
        case tokenTypeInt:
            return "Int";
        case tokenTypeKeyword:
            return "Keyword";
        case tokenTypeSign:
            return "Sign";
        case tokenTypeString:
            return "String";
        case tokenTypeUnknown:
            return "Unknown";
    }
    return "What?";
}
#endif