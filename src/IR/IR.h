#ifndef _IR_H_
#define _IR_H_

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include "../Value/Value.h"

enum IROp {
    /*
    TIP ~n(n is a digit):
        ~n means the n-th of the stack (isn't a var).
        ~n's value poped when it is used.
    TIP $n(n is a digit):
        $n can be considered as a temp var.
        $n will be a register or a piece of memory in stack.
    
    Other Tip:
    imm -> Immediate
    iv -> id.variable
    if -> id.function (but use `iv`)
    t  -> temp (but use `iv`)
    reg -> register
    */
    Op_none,
    Op_pop_iv,
    Op_pop_reg,
    Op_push_imm,
    Op_push_iv,
    Op_push_reg,
    Op_mov_iv_imm,
    Op_mov_reg_reg,
    Op_add, // push (~1 + ~0)
    Op_add_reg_reg,
    Op_sub, // push (~1 - ~0)
    Op_sub_reg_reg,
    Op_mul, // push (~1 * ~0)
    Op_mul_reg_reg,
    Op_div, // push (~1 / ~0)
    Op_mod,
    Op_idiv_val,
    //Op_cdq,
    Op_cqo,
    Op_call_if,
    Op_equal, // push (~1 == ~0)
    Op_equal_reg_reg,
    Op_bigger, // push (~1 > ~0)
    Op_bigger_reg_reg,
    Op_biggerEqual, // push (~1 >= ~0)
    Op_biggerEqual_reg_reg,
    Op_smaller, // push (~1 < ~0)
    Op_smaller_reg_reg,
    Op_smallerEqual, // push (~1 <= ~0)
    Op_smallerEqual_reg_reg,
    Op_notEqual, // push (~1 != ~0)
    Op_notEqual_reg_reg,

    Op_increment_iv,
    Op_decrement_iv,
    Op_add_iv, // iv += (~0)
    Op_sub_iv, // iv -= (~0)
    Op_mul_iv, // iv *= (~0)

    Op_negative,  // push -(~0)
    Op_neg_reg,

    // double float
    Op_addsd_reg_reg,
    Op_subsd_reg_reg,
    Op_mulsd_reg_reg,
    Op_divsd_reg_reg,
    //Op_equalsd_reg_reg,
    //Op_biggersd_reg_reg,
    //Op_biggerEqualsd_reg_reg,
    //Op_smallersd_reg_reg,
    //Op_smallerEqualsd_reg_reg,
    //Op_notEqualsd_reg_reg,

    // Other
    Op_return, // return (~0)
    Op_return_with_nothing,
    Op_return_imm,
    Op_return_reg,

    Op_power, // push (~1 ** ~0)

    Op_jump_addr, // jump to addr 
    Op_jumpIfNot_addr, // jump to addr if not ~0
    Op_jumpIf_addr, // jump to imm0 if not ~0

    Op_jumpIfNot_addr_reg,
    Op_jumpIf_addr_reg,

    Sign_callParaBegin, // as a sign to mark the beginning
    
    Op_load_iv_reg,
    Op_load_imm_reg,
    Op_load_mem_reg, // mov reg, [rsp + backup_offset]
    Op_store_iv_reg,

    Sign_newFunction_iv, // iv is the name
    Sign_endFunction,
    Sign_defineVariable_type_iv,
    Sign_SentenceEnd,
};


class IR {
    public:
    IROp op;
    Address addr;
    Value val0;
    Value val1;
    IR() = default;
    IR(IROp o) : op(o) {}
    IR(IROp o, Value v0) : op(o), val0(v0) {}
    IR(IROp o, Value v0, Value v1) : op(o), val0(v0), val1(v1) {}
    IR(IROp o, Address n) : op(o), addr(n) {}
    IR(IROp o, Address n, Value reg) : op(o), addr(n), val0(reg) {}
    void set_addr(Address addr);
    Address get_addr() const;
    #ifdef DEBUG
    void display(int count) const;
    #endif
};

class IRs {
    public:
    int pos = 0; // the position which will be on the next IR
    int getPosition();
    std::vector<IR> content;
    std::unordered_map<int, int> marks;
    
    int add(IR ir); // return the position of IR
    void replace(IRs & new_);
    //void
    #ifdef DEBUG
    void display();
    #endif
};
#endif
