#ifndef _SYNTAX_H_
#define _SYNTAX_H_
#include "../IR/IR.h"
#include "../Tree/Tree.h"
#include "AutoAssign/AutoAssign.h"
#include "../Value/Value.h"
#include "../Symbol/Symbol.h"

class Syntax {
    private:
    Tree * tr; // the Tree need to deal with
    IRs * irs;   // the result
    Symbol * symbol;
    // for jumping
    AutoAssign * start; // start of a loop
    AutoAssign * end;
    #ifdef DEBUG
    int count = 0;
    #endif
    public:
    Syntax(Tree * tr, IRs * irs, Symbol * symbol);
    ~Syntax();
    void init();
    // analyze
    int analyze_Immediate(Token * tk); // return the index
    void analyze_unit(); // run this to start working
    Token getInner(Tree * tr);
    void analyze_Statements(Tree * tr);
    void analyze_Sentences(Tree * tr);
    void analyze_Sentence(Tree * tr);
    void analyze_If(Tree * tr);
    void analyze_DoWhile(Tree * tr);
    void analyze_For(Tree * tr);
    void analyze_Else(Tree * tr, int if_jump_pos);
    void analyze_While(Tree * tr);
    void analyze_Break();
    void analyze_Return(Tree * tr);
    void analyze_Continue();
    void analyze_FunctionCall(Tree * tr);
    void analyze_ExprList(Tree * tr);
    void analyze_Expr(Tree * tr);
    void analyze_Assign(Tree * tr);
    void analyze_Compare(Tree * tr);
    void analyze_Add(Tree * tr);
    void analyze_Times(Tree * tr);
    void analyze_Power(Tree * tr);
    void analyze_Factor(Tree * tr);
    void analyze_Increment(Tree * tr);
    void analyze_Negative(Tree * tr);

    void analyze_DefineVariable(Tree * tr);
    void analyze_DefineVariableList(Tree * tr);
    void analyze_DefineFunction(Tree * tr);

    int append(const IR & ir);
    
};

#endif
