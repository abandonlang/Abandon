#include "StackEraser.h"
#include <array>
#include <algorithm>
#include <unordered_map>
#include "../SayError/SayError.h"

bool StackEraser::isRegUsed(char * reg_addr) {
    if (this->regs_state.contains(reg_addr)) {
        return this->regs_state[reg_addr];
    }
    return false;
}

void StackEraser::markUsed(char * reg_addr) {
    this->regs_state.insert_or_assign(reg_addr, true);
}
void StackEraser::releaseReg(Value reg) {
    char * reg_addr = reg.getReg();
    // two cases [real reg] [virtual reg in stack]
    if ((std::ranges::find(general_regs, reg_addr) != general_regs.end()) 
        || (std::ranges::find(xmm_regs, reg_addr) != xmm_regs.end())) {
        this->regs_state.insert_or_assign(reg_addr, false);
    } else {
        // TODO
    }
}
Value StackEraser::getReg() {
    for (auto reg_addr : general_regs) {
        if (!this->isRegUsed(reg_addr)) {
            this->markUsed(reg_addr);
            return Value(reg_addr);
        }
    }
    sayError("Spill is not supported now.");
    return Value(); // TODO
}
Value StackEraser::getFloatReg() {
    for (auto reg_addr : xmm_regs) {
        if (!this->isRegUsed(reg_addr)) {
            this->markUsed(reg_addr);
            return Value(reg_addr);
        }
    }
    sayError("Spill is not supported now.");
    return Value(); // TODO
}
Value StackEraser::getReg(Value from) {
    if (this->isFloat(from)) {
        return this->getFloatReg();
    }
    return this->getReg();
}

Value StackEraser::loadToReg(Value t) {
    return this->loadToReg(t, this->getReg(t));
}
Value StackEraser::loadToReg(Value t, Value reg) {
    return this->loadToReg(t, reg, false);
}
Value StackEraser::loadToReg(Value t, Value reg, bool isMustToReg) {
    IR ir;
    if (t.isVariable()) {
        this->append({Op_load_iv_reg, t, reg});
    } else if (t.isImmediate()) {
        this->append({Op_load_imm_reg, t, reg});
    } else if (t.isReg()) {
        if (isMustToReg) {
            this->append({Op_mov_reg_reg, reg, t});
        } else {
            this->releaseReg(reg);
            return t;
        }
    }
    return reg;
}

void StackEraser::protectReg(char * reg_addr) {
    this->append({Op_push_reg, reg_addr});
    this->releaseReg(reg_addr);
    this->protected_reg_list.push_back(reg_addr);
}
void StackEraser::restoreRegs() {
    for (auto it = this->protected_reg_list.rbegin(); it != this->protected_reg_list.rend(); ++ it) {
        this->append({Op_pop_reg, *it});
        this->markUsed(*it);
    }
    this->protected_reg_list.clear();
}

void StackEraser::evacuateReg(char * reg_addr) {
    Value new_ = loadToReg(reg_addr, this->getReg(), true);
    for (auto it = this->stack.begin(); it != this->stack.end(); ++ it) {
        if (*it == Value(reg_addr)) {
            *it = new_;
        }
    }
    this->releaseReg(reg_addr);
}