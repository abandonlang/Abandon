#include "StackEraser.h"
#include <string>
#include <algorithm>
#include "../SayError/SayError.h"
#ifdef DEBUG
#include <iostream>
#endif

StackEraser::StackEraser(IRs * irs, Symbol * symbol) {
    this->old = irs;
    this->irs = new IRs();
    this->Handle_sign_sentence_end({});
    this->symbol = symbol;
}

inline bool StackEraser::isStackUsed(int n) {
    return this->stack_used.find(n) != this->stack_used.end();
}

int StackEraser::getStack() {
    int i = (-1);
    while (this->isStackUsed(i)) {
        -- i;
    }
    this->stack_used.insert(i);
    return i;
}

void StackEraser::releaseStack(int n){
    if (this->isStackUsed(n)) {
        this->stack_used.erase(n);
    }
}

StackEraser::~StackEraser() {
    delete this->irs;
}

Value StackEraser::pop() {
    Value v;
    v = this->stack.back();
    this->stack.pop_back();
    return v;
}
void StackEraser::push(const Value & v) {
    this->stack.push_back(v);
}
void StackEraser::append(const IR & ir) {
    #ifdef DEBUG
    ir.display(this->n);
    #endif
    this->irs->add(ir);
}
void StackEraser::Handle_pop_iv(const IR & i) {
    Value v = this->pop();
    if (v.isVariable()) {
        Value reg = this->getReg(v);
        this->append({Op_load_iv_reg, v, reg});
        this->append({Op_store_iv_reg, i.val0, reg});
        this->releaseReg(reg);
    } else if (v.isImmediate()) {
        if (this->isFloat(v)) {
            Value v_reg = this->getFloatReg();
            this->append({Op_load_imm_reg, v, v_reg});
            this->append({Op_store_iv_reg, i.val0, v_reg});
            this->releaseReg(v_reg);
            return ;
        }
        this->append({Op_mov_iv_imm, i.val0, v});
    } else if (v.isReg()) {
        this->append({Op_store_iv_reg, i.val0, v});
        this->releaseReg(v);
    }
    
}
void StackEraser::Handle_push_imm(const IR & i) {
    this->push(Value(i.val0));
}
void StackEraser::Handle_push_iv(const IR & i) {
    this->push(Value(i.val0));
}
void StackEraser::Handle_xxx(const IR & i) {
    IROp op = Op_none;
    Value a_reg = this->loadToReg(this->pop());
    Value b_reg = this->loadToReg(this->pop());
    // check if b_reg is a float
    if (std::ranges::find(xmm_regs, b_reg.getReg()) != xmm_regs.end()) {
        switch (i.op) {
            case Op_add:
                this->append({Op_addsd_reg_reg, b_reg, a_reg});
                break;
            case Op_sub:
                this->append({Op_subsd_reg_reg, b_reg, a_reg});
                break;
            case Op_mul:
                this->append({Op_mulsd_reg_reg, b_reg, a_reg});
                break;
            default:
                sayError("Float is not support this operating.");
        }
        this->releaseReg(a_reg);
        this->push(b_reg);
        return ;
    }
    // if not, then
    switch (i.op) {
        case Op_add: op = Op_add_reg_reg;  break;
        case Op_sub: op = Op_sub_reg_reg;  break;
        case Op_mul: op = Op_mul_reg_reg;  break;
        case Op_equal: op = Op_equal_reg_reg;  break;
        case Op_bigger: op = Op_bigger_reg_reg;  break;
        case Op_biggerEqual: op = Op_biggerEqual_reg_reg;  break;
        case Op_smaller: op = Op_smaller_reg_reg;  break;
        case Op_smallerEqual: op = Op_smallerEqual_reg_reg;  break;
        case Op_notEqual: op = Op_notEqual_reg_reg;  break;
        default: break;
    }
    this->append({op, b_reg, a_reg});
    this->releaseReg(a_reg);
    this->push(b_reg);
}
void StackEraser::Handle_xxx_iv(const IR & ir) {
    IROp op = Op_none;
    switch (ir.op) {
        case Op_add_iv: op = Op_add_reg_reg; break;
        case Op_sub_iv: op = Op_sub_reg_reg; break;
        case Op_mul_iv: op = Op_mul_reg_reg; break;
        default: break;
    }
    Value v0_reg = this->loadToReg(ir.val0);
    Value v1_reg = this->loadToReg(this->pop());
    this->append({op, v0_reg, v1_reg});
    this->append({Op_store_iv_reg, ir.val0, v0_reg});
    this->releaseReg(v0_reg);
    this->releaseReg(v1_reg);
}
void StackEraser::Handle_negative(const IR & ir) {
    (void)ir;
    Value v = this->pop();
    if (v.isImmediate()) { // if is -n (e.g. -1024)
        Immediate imm = v.getImmediate();
        this->push(makeImmediate(imm.type, "-" + imm.content));
        return ;
    }
    // else
    Value v_reg = this->loadToReg(v);
    this->append({Op_neg_reg, v_reg});
    this->push(v_reg);
}
void StackEraser::Handle_div(const IR & i) {
    (void)i;
    // a / b
    Value b = this->pop();
    Value a = this->pop();
    // check a is a float
    if (this->isFloat(a)) {
        Value a_reg = this->loadToReg(a);
        Value b_reg = this->loadToReg(b);
        this->append({Op_divsd_reg_reg, a_reg, b_reg});
        this->releaseReg(b_reg);
        this->push(a_reg);
        return ;
    }
    // if not, then
    bool isPopRax = false, isPopRdx = false;
    if (a != Value(rax)) {
        if (this->isRegUsed(rax)) {
            this->append({Op_push_reg, Value(rax)});
            isPopRax = true;
        }
        this->loadToReg(a, Value(rax));
    }
    if (this->isRegUsed(rdx)) {
        this->append({Op_push_reg, Value(rdx)});
        isPopRdx = true;
    }
    this->markUsed(rax);
    this->markUsed(rdx);
    this->append({Op_cqo});
    if (b.isImmediate()) {
        Value b_reg = this->loadToReg(b);
        this->append({Op_idiv_val, b_reg});
        this->releaseReg(b_reg);
    } else {
        this->append({Op_idiv_val, b});
    }
    if (isPopRdx) {
        this->append({Op_pop_reg, Value(rdx)});
    } else {
        this->releaseReg(rdx);
    }
    if (isPopRax) {
        this->push(this->loadToReg(Value(rax)));
        this->append({Op_pop_reg, Value(rax)});
    } else {
        this->push(Value(rax));
    }
}
void StackEraser::Handle_mod(const IR & ir) {
    (void)ir;
    // a % b
    Value b = this->pop();
    Value a = this->pop();
    bool isPopRax = false, isPopRdx = false;
    if (a != Value(rax)) {
        if (this->isRegUsed(rax)) {
            this->append({Op_push_reg, Value(rax)});
            isPopRax = true;
        }
        this->loadToReg(a, Value(rax));
    }
    if (this->isRegUsed(rdx)) {
        this->append({Op_push_reg, Value(rdx)});
        isPopRdx = true;
    }
    this->markUsed(rax);
    this->markUsed(rdx);
    this->append({Op_cqo});
    if (b.isImmediate()) {
        Value b_reg = this->loadToReg(b);
        this->append({Op_idiv_val, b_reg});
        this->releaseReg(b_reg);
    } else {
        this->append({Op_idiv_val, b});
    }
    if (isPopRdx) {
        this->push(this->loadToReg(Value(rdx)));
        this->append({Op_pop_reg, Value(rdx)});
    } else {
        this->push(Value(rdx));
    }
    if (isPopRax) {
        this->append({Op_pop_reg, Value(rax)});
    } else {
        this->releaseReg(rax);
    }
}
void StackEraser::Handle_power(const IR & ir) {
    (void)ir;
    Value b = this->pop();
    Value a = this->pop();
    this->Handle_callParaBegin({});
    this->push(a);
    this->push(b);
    this->Handle_call_if({Op_call_if, Value(std::string("pow"))});
}
void StackEraser::Handle_conditionJump_addr(const IR & i) {
    IROp op = Op_none;
    switch (i.op) {
        case Op_jumpIf_addr: op = Op_jumpIf_addr_reg; break;
        case Op_jumpIfNot_addr: op = Op_jumpIfNot_addr_reg; break;
        default: break;
    }
    Value a_reg = this->loadToReg(this->pop());
    this->append({op, i.get_addr(), a_reg});
    this->releaseReg(a_reg);
}
void StackEraser::Handle_callParaBegin(const IR & i) {
    (void)i;
    this->push(Value(static_cast<SpecialMark>(FUNCTION_CALL_PARA_HEAD)));
}
void StackEraser::Handle_return(const IR & i) {
    (void)i;
    Value v = this->pop();
    if (v.isImmediate()) {
        this->append({Op_return_imm, v});
        return ;
    }
    Value v_reg = this->loadToReg(v);
    this->append({Op_return_reg, v_reg});
}
void StackEraser::Handle_sign_sentence_end(const IR & ir) {
    (void)ir;
    this->stack = {};
    for (auto reg_addr : general_regs) {
        this->releaseReg(reg_addr);
    }
    for (auto reg_addr : xmm_regs) {
        this->releaseReg(reg_addr);
    }
}
void StackEraser::convert() {
    n = 0; // this->n
    #ifdef DEBUG
    std::cout << "---------------" << std::endl;
    #endif
    Value v;
    using OpHandler = void (StackEraser::*)(const IR&);
    std::unordered_map<IROp, OpHandler> handlers = {
        {Op_pop_iv, &StackEraser::Handle_pop_iv},
        {Op_push_imm, &StackEraser::Handle_push_imm},
        {Op_push_iv, &StackEraser::Handle_push_iv},
        {Op_add, &StackEraser::Handle_xxx},
        {Op_sub, &StackEraser::Handle_xxx},
        {Op_mul, &StackEraser::Handle_xxx},
        {Op_div, &StackEraser::Handle_div},
        {Op_mod, &StackEraser::Handle_mod},
        {Op_negative, &StackEraser::Handle_negative},
        {Op_equal, &StackEraser::Handle_xxx},
        {Op_bigger, &StackEraser::Handle_xxx},
        {Op_biggerEqual, &StackEraser::Handle_xxx},
        {Op_smaller, &StackEraser::Handle_xxx},
        {Op_smallerEqual, &StackEraser::Handle_xxx},
        {Op_notEqual, &StackEraser::Handle_xxx},
        {Op_power, &StackEraser::Handle_power},
        {Op_add_iv, &StackEraser::Handle_xxx_iv},
        {Op_sub_iv, &StackEraser::Handle_xxx_iv},
        {Op_mul_iv, &StackEraser::Handle_xxx_iv},
        {Op_jumpIf_addr, &StackEraser::Handle_conditionJump_addr},
        {Op_jumpIfNot_addr, &StackEraser::Handle_conditionJump_addr},
        {Sign_callParaBegin, &StackEraser::Handle_callParaBegin},
        {Op_call_if, &StackEraser::Handle_call_if},
        {Op_return, &StackEraser::Handle_return},
        {Sign_SentenceEnd, &StackEraser::Handle_sign_sentence_end},
    };
    for (IR i : this->old->content) {
        this->lineCast.insert({n, this->irs->pos});
        auto it = handlers.find(i.op);
        if (it != handlers.end()) {
            (this->*(it->second))(i);
        } else {
            this->append(i);
        }
        n ++;
    }
    this->lineCast.insert({n, this->irs->pos});
    this->replaceLineNumber();
    this->old->replace(*this->irs);
}
void StackEraser::replaceLineNumber() {
    int size = this->irs->content.size();
    int count = 0;
    for (IR & i : this->irs->content) {
        switch (i.op) {
            case Op_jump_addr: // fall through
            case Op_jumpIf_addr_reg:
            case Op_jumpIfNot_addr_reg: {
                int line = this->lineCast.find(i.get_addr().line)->second;
                if (line >= size) {
                    this->irs->content.push_back({Op_none});
                    size ++;
                }
                // when meet line, show L{count}:
                this->irs->marks.insert({line, count});
                count ++;
                i.set_addr(Address(line));
                break;
            }
            default: break;
        }
    }
}
