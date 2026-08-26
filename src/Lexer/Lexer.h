#ifndef _LEXER_H_
#define _LEXER_H_

#include "../FileManager/FileManager.h"
#include "../Token/Token.h"

enum signState {
    signStateRight,
    signStateWrong,
    signStateComment
};

class Lexer {
    private:

    FileManager* file;
    Token intToken();
    Token idToken();
    char readChar(char cannotBe, bool& tell); // is char is CB, then tell'll be true
    Token charToken();
    Token stringToken();
    Token signToken();
    bool isSpace(char ch);
    bool isDigit(char ch);
    bool isLetter(char ch);
    bool isSign(char ch);

    Token _peek = Token();
    
    public:
    Token current;
    void setFile(FileManager& file);
    Token getNextToken();
    Token peek();
    enum signState isSignTwoChars(char a, char b);
};
#endif
