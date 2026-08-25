#include "Syntax.h"
#include <vector>

TypeType typeConvert(std::string t) {
    if (t == "int") return TYPE_INT;
    else if (t == "float") return TYPE_FLOAT;
    else if (t == "char") return TYPE_CHAR;
    else if (t == "string") return TYPE_STRING;
    else if (t == "void") return TYPE_VOID;
    return TYPE_UNKNOWN;
}

void Syntax::analyze_DefineVariable(Tree * tr) {
    TypeType type0;
    std::string t = tr->children[0]->tk.content;
    std::string var_name = tr->children[1]->tk.content;

    type0 = typeConvert(t);

    this->append({Sign_defineVariable_type_iv, Value(type0), Value(var_name)});
}

void Syntax::analyze_DefineVariableList(Tree * tr) {
    for (Tree * DefineVariable : tr->children) {
        this->analyze_DefineVariable(DefineVariable);
    }
}

std::vector<FunctionArg> DefineVariableList2Types(Tree * tr) {
    std::vector<FunctionArg> result;
    for (Tree * tr_ : tr->children) {
        result.push_back({
            .type = typeConvert(tr_->children[0]->tk.content),
            .name = tr_->children[1]->tk.content
        });
    }
    return result;
}

void Syntax::analyze_DefineFunction(Tree * tr) {
    // fn (extern) int main (void) {}
    SymbolValue sv;
    std::string func_name = tr->children[1]->tk.content;
    sv.isVariable = false;
    sv.isExist = true;
    sv.type = typeConvert(tr->children[0]->tk.content);
    sv.args = DefineVariableList2Types(tr->children[2]);
    this->symbol->insert_function(func_name, sv);
    if (tr->label == treeTypeNode_FunctionExtern) {
        return ;
    }
    this->append({Sign_newFunction_iv, func_name});
    //this->analyze_DefineVariableList(tr->children[2]);
    this->analyze_Statements(tr->children[3]);
    // check if has return sentence
    if (this->irs->content.empty() 
    || this->irs->content.back().op != Op_return_with_nothing 
    || this->irs->content.back().op != Op_return) {
        this->append({Op_return_with_nothing});
    }
    this->append({Sign_endFunction});
}