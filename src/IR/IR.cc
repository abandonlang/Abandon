#include "IR.h"
#ifdef DEBUG
#include <iostream>
#endif


void IR::set_addr(Address addr) {
    this->addr = addr;
}

Address IR::get_addr() const {
    return this->addr;
}

int IRs::getPosition() {
    this->pos ++;
    return (this->pos - 1);
}

int IRs::add(IR ir) {
    int pos = this->getPosition();
    this->content.push_back(std::move(ir));
    return pos;
}

void IRs::replace(IRs & new_) {
    this->content = new_.content;
    this->pos = this->content.size();
    this->marks = new_.marks;
}

#ifdef DEBUG
std::string IROpToText(IROp n) {
    switch (n) {
        case Op_none: return "none";
        case Op_pop_iv: return "pop_iv";
        case Op_push_imm: return "push_imm";
        case Op_push_iv: return "push_iv";
        case Op_push_reg: return "push_reg";
        case Op_mov_iv_imm: return "mov_iv_imm";
        case Op_mov_reg_reg: return "mov_reg_reg";
        case Op_add: return "add";
        case Op_add_reg_reg: return "add_reg_reg";
        case Op_sub: return "sub";
        case Op_sub_reg_reg: return "sub_reg_reg";
        case Op_mul: return "mul";
        case Op_mul_reg_reg: return "mul_reg_reg";
        case Op_div: return "div";
        case Op_idiv_val: return "idiv_val";
        case Op_call_if: return "call_if";
        case Op_equal: return "equal";
        case Op_bigger: return "bigger";
        case Op_biggerEqual: return "biggerEqual";
        case Op_smaller: return "smaller";
        case Op_smallerEqual: return "smallerEqual";
        case Op_notEqual: return "notEqual";
        case Op_power: return "power";
        case Op_jump_addr: return "jump_addr";
        case Op_jumpIfNot_addr: return "jumpIfNot_addr";
        case Op_jumpIf_addr: return "jumpIf_addr";
        case Sign_callParaBegin: return "callParaBegin";
        case Op_load_iv_reg: return "load_iv_reg";
        case Op_load_imm_reg: return "load_imm_reg";
        case Op_store_iv_reg: return "store_iv_reg";
        case Op_equal_reg_reg:  return "equal_reg_reg";
        case Op_bigger_reg_reg:  return "bigger_reg_reg";
        case Op_biggerEqual_reg_reg:  return "biggerEqual_reg_reg";
        case Op_smaller_reg_reg:  return "smaller_reg_reg";
        case Op_smallerEqual_reg_reg:  return "smallerEqual_reg_reg";
        case Op_notEqual_reg_reg:  return "notEqual_reg_reg";
        case Op_jumpIf_addr_reg: return "jump_addr_reg";
        case Op_jumpIfNot_addr_reg: return "jumpIfNot_addr_reg";
        case Sign_defineVariable_type_iv: return "defineVariable_type_iv";
        case Op_pop_reg: return "pop_reg";
        case Op_addsd_reg_reg: return "addsd_reg_reg";
        case Op_subsd_reg_reg: return "subsd_reg_reg";
        case Op_mulsd_reg_reg: return "mulsd_reg_reg";
        case Op_divsd_reg_reg: return "divsd_reg_reg";
        /*case Op_equalsd_reg_reg: return "equalsd_reg_reg";
        case Op_biggersd_reg_reg: return "biggersd_reg_reg";
        case Op_biggerEqualsd_reg_reg: return "biggerEqualsd_reg_reg";
        case Op_smallersd_reg_reg: return "smallersd_reg_reg";
        case Op_smallerEqualsd_reg_reg: return "smallerEqualsd_reg_reg";
        case Op_notEqualsd_reg_reg: return "notEqualsd_reg_reg";
        */
        case Op_return: return "return";
        case Op_return_imm: return "return_imm";
        case Op_return_reg: return "return_reg";
        case Sign_newFunction_iv: return "newFunction_iv";
        case Sign_endFunction: return "endFunction";
        case Sign_SentenceEnd: return "SentenceEnd";
        case Op_cqo: return "cqo";
        case Op_mod: return "mod";
        case Op_increment_iv: return "increment_iv";
        case Op_decrement_iv: return "decrement_iv";
        case Op_add_iv: return "add_iv";
        case Op_sub_iv: return "sub_iv";
        case Op_mul_iv: return "mul_iv";
        case Op_negative: return "negative";
        case Op_neg_reg: return "neg_reg";
        case Op_load_mem_reg: return "load_mem_reg";
        case Op_return_with_nothing: return "return_with_nothing";
    }
    return "Error: IR.cc::IRs::display();\n";
}
void IR::display(int count) const {
    auto & ir = *this;
    std::cout << count << ":\t" << IROpToText(ir.op) << "\t";
    if (IROpToText(ir.op).find("jump") != std::string::npos) {
        std::cout << ir.addr.line << "\t";
    }
    std::cout << ir.val0.toString() << "\t" << ir.val1.toString() << std::endl;
}
void IRs::display() {
    int count = 0;
    for (auto ir : this->content) {
        ir.display(count);
        ++ count;
    }
    return ;
}

#endif
