#include "StackEraser.h"
#include "../SayError/SayError.h"
#include <array>
#include <format>
#include <unordered_map>
#include <vector>
#include <algorithm>

void StackEraser::Handle_call_if(const IR & i) {
    std::string func_name = i.val0.getIdVariable().content;
    SymbolValue func = this->symbol->get(func_name);
    if (func.isExist == false) {
        sayError(std::format("`{}` is not exist as a function.", func_name));
    }
    if (func.isVariable == true) {
        sayError(std::format("`{}` is a variable name.", func_name));
    }
    // deal with parameters
    std::vector<Value> parameters; // reverse of real parameters
    Value para;
    while (!(para = this->pop()).isParaHead()) {
        parameters.push_back(para);
    }
    // check types of argument
    auto origin_para = func.args.begin();
    for (auto it = parameters.rbegin();  it != parameters.rend();  ++ it) {
        if (origin_para == func.args.end()) {
            sayError("Too many args");
            break;
        }
        if (origin_para->type != this->getValueType(*it)) {
            sayError(std::format(
                "Wrong arg type, you used `{}`. But should be `{}`.",
                TypeTypeToString(this->getValueType(*it)),
                TypeTypeToString(origin_para->type)));
        }
        ++ origin_para;
    }
    // save rax
    if (this->isRegUsed(rax)) {
        this->evacuateReg(rax);
    }
    // caller save
    int save_count = 0;
    std::unordered_map<char*, int> offsets;// reg --> offset of save_count
    for (auto reg_addr : caller_saved) {
        if (this->isRegUsed(reg_addr)) {
            this->protectReg(reg_addr);
            save_count += 8; // TODO 
            offsets.insert_or_assign(reg_addr, -save_count);
        }
    }
    // Classification
    std::vector<Value> INTEGER;
    std::vector<Value> SSE;
    std::vector<Value> MEMORY;
    // parameters left --> right (register-only)
    for (auto it = parameters.rbegin(); it != parameters.rend(); ++ it) {
        if (this->isFloat(*it)) {
            if (SSE.size() == SSE_passing.size()) MEMORY.push_back(*it);
            else SSE.push_back(*it);
        } else {
            if (INTEGER.size() == INTEGER_passing.size()) MEMORY.push_back(*it);
            else INTEGER.push_back(*it);
        }
    }
    std::reverse(MEMORY.begin(), MEMORY.end());
    // Parameters Passing
    // INTEGER passing
    {
        auto it = INTEGER_passing.begin();
        for (auto p : INTEGER) {
            if (p.isReg() && std::ranges::find(INTEGER_passing, p.getReg()) != INTEGER_passing.end()) {
                // load from stack
                this->append({Op_load_mem_reg, Value(save_count+offsets.at(p.getReg())), *it});
            } else {
                this->loadToReg(p, *it, true);
            }
            ++ it;
        }
    }
    // SSE passing
    {
        auto it = SSE_passing.begin();
        for (auto p : SSE) {
            if (p.isReg() && std::ranges::find(SSE_passing, p.getReg()) != SSE_passing.end()) {
                // load from stack
                this->append({Op_load_mem_reg, Value(save_count+offsets.at(p.getReg())), *it});
            } else {
                this->loadToReg(p, *it, true);
            }
            ++ it;
        }
    }
    // call
    this->append({Op_call_if, func_name});
    // end
    this->restoreRegs();
    this->push(rax);
    this->markUsed(rax);
}

