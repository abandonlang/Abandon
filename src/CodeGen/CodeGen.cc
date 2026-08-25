#include "CodeGen.h"
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <format>
#include "../SayError/SayError.h"

std::unordered_map<char*, std::string> low8_map = {
    {rax, "al"},
    {rcx, "cl"},
    {rdx, "dl"},
    {rbx, "bl"},
    {rsp, "spl"},
    {rbp, "bpl"},
    {rsi, "sil"},
    {rdi, "dil"},
    {r8,  "r8b"},
    {r9,  "r9b"},
    {r10, "r10b"},
    {r11, "r11b"},
    {r12, "r12b"},
    {r13, "r13b"},
    {r14, "r14b"},
    {r15, "r15b"}
};

CodeGen::CodeGen(IRs * irs, Symbol * symbol) {
    this->irs = irs;
    this->symbol = symbol;
}

void CodeGen::append(std::string ins) {
    this->_output.back().code << ins << '\n';
}
std::string CodeGen::get_output() {
    std::string ret;
    ret += "section .rodata\n" +
            this->literal.get_rodata() +
            "section .text\n"
            "extern print\n"
            "extern input\n"
            "extern pow\n"
            "extern print_float\n";
    for (FuncData & _o : this->_output) {
        ret += std::format(
            "\n"
            "global {}\n"
            "{}:\n"
            "push rbp\n"
            "mov rbp, rsp\n"
            "sub rsp, {}\n"
            "{}"
        , _o.name, _o.name, _o.allocate, _o.code.str());
    }
    return ret;
}

void CodeGen::Handle_newFunction_iv(const IR & ir) {
    std::string func_name = ir.val0.getIdVariable().content;
    this->symbol->new_scope();
    this->_output.push_back(FuncData{
        .allocate=0, // not important
        .name=func_name,
        .code=std::stringstream()
    });
    SymbolValue func = this->symbol->get(func_name);
    // rdi, rsi, rdx, rcx, r8, r9
    auto int_arg = INTEGER_passing.begin();
    auto float_arg = SSE_passing.begin();
    for (const FunctionArg & arg : func.args) {
        if (arg.type == TYPE_INT) {
            this->symbol->insert_variable(arg.name, TYPE_INT);
            this->append(std::format(
                "mov {}, {}",
                this->symbol->get_variable_mem(arg.name),
                *int_arg
            ));
            ++ int_arg;
        } else if (arg.type == TYPE_FLOAT) {
            this->symbol->insert_variable(arg.name, TYPE_FLOAT);
            this->append(std::format(
                "movsd {}, {}",
                this->symbol->get_variable_mem(arg.name),
                *float_arg
            ));
            ++ float_arg;
        }
    }
}

void CodeGen::Handle_endFunction(const IR & ir) {
    (void)ir;
    int allocate = -this->symbol->exit_scope();
    allocate = (allocate + 15) & ~15;
    this->_output.back().allocate = allocate;
}
void CodeGen::Handle_sign_defineVariable_type_iv(const IR & ir) {
    TypeType var_type = ir.val0.getType();
    std::string var_name = ir.val1.getIdVariable().content;
    this->symbol->insert_variable(var_name, var_type);
}
void CodeGen::Handle_mov_iv_imm(const IR & ir) {
    std::string opcode;
    if (this->symbol->get_variable(ir.val0).type == TYPE_FLOAT) {
        opcode = "movsd";
    } else {
        opcode = "mov";
    }
    this->append(std::format("{} {}, {}",
        opcode,
        this->symbol->get_variable_mem(ir.val0),
        this->literal.get(ir.val1)
    ));
}
void CodeGen::Handle_xxx_reg_reg(const IR & ir) {
    std::string opcode;
    switch (ir.op) {
        case Op_mov_reg_reg: {
            if (ir.val0.isRegFloat()) {
                opcode = "movsd";
            } else {
                opcode = "mov";
            }
            break;
        }
        case Op_add_reg_reg: opcode = "add"; break;
        case Op_sub_reg_reg: opcode = "sub"; break;
        case Op_mul_reg_reg: opcode = "imul"; break;
        default: break;
    }
    this->append(opcode + ' ' + ir.val0.getReg() + ", " + ir.val1.getReg());
}
void CodeGen::Handle_xxxsd_reg_reg(const IR & ir) {
    std::string opcode;
    switch (ir.op) {
        case Op_addsd_reg_reg: opcode = "add"; break;
        case Op_subsd_reg_reg: opcode = "sub"; break;
        case Op_mulsd_reg_reg: opcode = "mul"; break;
        case Op_divsd_reg_reg: opcode = "div"; break;
        default:
            sayError("CodeGen::Handle_xxxsd_reg_reg()");
    }
    this->append(std::format("{}sd {}, {}",
        opcode,
        ir.val0.getReg(),
        ir.val1.getReg()
    ));
}
void CodeGen::Handle_idiv_val(const IR & ir) {
    if (ir.val0.isReg()) {
        this->append(std::string("idiv ") + ir.val0.getReg());
    } else if (ir.val0.isImmediate()) {
        this->append("idiv " + ir.val0.getImmediate().content);
    } else if (ir.val0.isVariable()) {
        this->append("idiv " + this->symbol->get_variable_mem(ir.val0));
    }else {
        sayError("Unkown type");
    }
}
void CodeGen::Handle_cqo(const IR & ir) {
    (void)ir;
    this->append("cqo");
}
void CodeGen::Handle_load_imm_reg(const IR & ir) {
    std::string opcode;
    if (ir.val0.getImmediate().type == TYPE_FLOAT) {
        opcode = "movsd";
    } else {
        opcode = "mov";
    }
    this->append(std::format("{} {}, {}",
        opcode,
        ir.val1.getReg(),
        this->literal.get(ir.val0)
    ));
}
void CodeGen::Handle_load_iv_reg(const IR & ir) {
    std::string opcode;
    if (this->symbol->get_variable(ir.val0).type == TYPE_FLOAT) {
        opcode = "movsd";
    } else {
        opcode = "mov";
    }
    this->append(std::format("{} {}, {}",
        opcode,
        ir.val1.getReg(),
        this->symbol->get_variable_mem(ir.val0))
    );
}
void CodeGen::Handle_load_mem_reg(const IR & ir) {
    std::string post = "";
    if (ir.val1.isRegFloat()) {
        post = "sd";
    }
    this->append(
        std::format("mov{} {}, [rsp + {}]",
            post,
            ir.val1.getReg(),
            ir.val0.getMem()
    ));
}
void CodeGen::Handle_store_iv_reg(const IR & ir) {
    std::string opcode;
    if (this->symbol->get_variable(ir.val0).type == TYPE_FLOAT) {
        opcode = "movsd";
    } else {
        opcode = "mov";
    }
    this->append(std::format("{} {},{}",
        opcode,
        this->symbol->get_variable_mem(ir.val0),
        ir.val1.getReg()
    ));
}
void CodeGen::Handle_jump_addr(const IR & ir) {
    this->append("jmp L" + std::to_string(this->irs->marks[ir.get_addr().line]));
}
void CodeGen::Handle_jumpIf_addr_reg(const IR & ir) {
    this->append(std::string("test ") + ir.val0.getReg() + ", " + ir.val0.getReg());
    this->append("jne L" + std::to_string(this->irs->marks[ir.get_addr().line]));
}
void CodeGen::Handle_jumpIfNot_addr_reg(const IR & ir) {
    this->append(std::string("test ") + ir.val0.getReg() + ", " + ir.val0.getReg());
    this->append("je L" + std::to_string(this->irs->marks[ir.get_addr().line]));
}
void CodeGen::Handle_compare_reg_reg(const IR & ir) {
    std::string opcode;
    std::string reg0 = ir.val0.getReg();
    std::string reg1 = ir.val1.getReg();
    std::string low8_reg0 = low8_map[ir.val0.getReg()];
    switch (ir.op) {
        case Op_equal_reg_reg: opcode = "sete"; break;
        case Op_bigger_reg_reg: opcode = "setg"; break;
        case Op_biggerEqual_reg_reg: opcode = "setge"; break;
        case Op_smaller_reg_reg: opcode = "setl"; break;
        case Op_smallerEqual_reg_reg: opcode = "setle"; break;
        case Op_notEqual_reg_reg: opcode = "setne"; break;
        default: break;
    }
    this->append("cmp " + reg0 + ", " + reg1);
    this->append(opcode + " " + low8_reg0);
    this->append("movzx " + reg0 + ", " + low8_reg0);
}
void CodeGen::Handle_push_imm(const IR & ir) {
    this->append("push " + ir.val0.getImmediate().content);
}
void CodeGen::Handle_push_iv(const IR & ir) {
    this->append("push " + this->symbol->get_variable_mem(ir.val0));
}
void CodeGen::Handle_push_reg(const IR & ir) {
    if (ir.val0.isRegFloat()) {
        this->append(std::format(
            "sub rsp, 16\n"
            "movsd [rsp], {}",
            ir.val0.getReg()
        ));
    } else {
        this->append(std::string("push ") + ir.val0.getReg());
    }
}
void CodeGen::Handle_pop_reg(const IR & ir) {
    if (ir.val0.isRegFloat()) {
        this->append(std::format(
            "movsd {}, [rsp]\n"
            "add rsp, 16",
            ir.val0.getReg()
        ));
    } else {
        this->append(std::string("pop ") + ir.val0.getReg());
    }
}
void CodeGen::Handle_pop_iv(const IR & ir) {
    this->append("pop " + this->symbol->get_variable_mem(ir.val0));
}
void CodeGen::Handle_call_if(const IR & ir) {
    this->append("call " + ir.val0.getIdVariable().content);
}
void CodeGen::Handle_return_with_nothing(const IR & ir) {
    (void)ir;
    this->append("leave");
    this->append("ret");
}
void CodeGen::Handle_return_imm(const IR & ir) {
    if (ir.val0.getImmediate().content == "0") {
        this->append("xor rax, rax");
    } else {
        this->append("mov rax, " + ir.val0.getImmediate().content);
    }
    this->append("leave");
    this->append("ret");
}
void CodeGen::Handle_return_reg(const IR & ir) {
    if (ir.val0.getReg() != rax) {
        this->append(std::string("mov rax, ") + ir.val0.getReg());
    }
    this->append("leave");
    this->append("ret");
}
void CodeGen::Handle_increment_iv(const IR & ir) {
    this->append(std::format(
        "inc {}",
        this->symbol->get_variable_mem(ir.val0)
    ));
}
void CodeGen::Handle_decrement_iv(const IR & ir) {
    this->append(std::format(
        "dec {}",
        this->symbol->get_variable_mem(ir.val0)
    ));
}
void CodeGen::Handle_neg_reg(const IR & ir) {
    this->append(std::format(
        "neg {}",
        ir.val0.getReg()
    ));
}

void CodeGen::generate() {
    int count = 0;
    using OpHandler = void (CodeGen::*)(const IR&);
    std::unordered_map<IROp, OpHandler> handlers = {
        {Sign_newFunction_iv, &CodeGen::Handle_newFunction_iv},
        {Sign_endFunction, &CodeGen::Handle_endFunction},
        {Sign_defineVariable_type_iv, &CodeGen::Handle_sign_defineVariable_type_iv},
        {Op_mov_iv_imm, &CodeGen::Handle_mov_iv_imm},
        {Op_mov_reg_reg, &CodeGen::Handle_xxx_reg_reg},
        {Op_add_reg_reg, &CodeGen::Handle_xxx_reg_reg},
        {Op_sub_reg_reg, &CodeGen::Handle_xxx_reg_reg},
        {Op_mul_reg_reg, &CodeGen::Handle_xxx_reg_reg},
        {Op_idiv_val, &CodeGen::Handle_idiv_val},
        {Op_cqo, &CodeGen::Handle_cqo},
        {Op_addsd_reg_reg, &CodeGen::Handle_xxxsd_reg_reg},
        {Op_subsd_reg_reg, &CodeGen::Handle_xxxsd_reg_reg},
        {Op_mulsd_reg_reg, &CodeGen::Handle_xxxsd_reg_reg},
        {Op_divsd_reg_reg, &CodeGen::Handle_xxxsd_reg_reg},
        {Op_load_imm_reg, &CodeGen::Handle_load_imm_reg},
        {Op_load_iv_reg, &CodeGen::Handle_load_iv_reg},
        {Op_load_mem_reg, &CodeGen::Handle_load_mem_reg},
        {Op_store_iv_reg, &CodeGen::Handle_store_iv_reg},
        {Op_jump_addr, &CodeGen::Handle_jump_addr},
        {Op_jumpIf_addr_reg, &CodeGen::Handle_jumpIf_addr_reg},
        {Op_jumpIfNot_addr_reg, &CodeGen::Handle_jumpIfNot_addr_reg},
        {Op_equal_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_bigger_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_biggerEqual_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_smaller_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_smallerEqual_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_notEqual_reg_reg, &CodeGen::Handle_compare_reg_reg},
        {Op_push_imm, &CodeGen::Handle_push_imm},
        {Op_push_iv, &CodeGen::Handle_push_iv},
        {Op_push_reg, &CodeGen::Handle_push_reg},
        {Op_pop_reg, &CodeGen::Handle_pop_reg},
        {Op_pop_iv, &CodeGen::Handle_pop_iv},
        {Op_call_if, &CodeGen::Handle_call_if},
        {Op_return_with_nothing, &CodeGen::Handle_return_with_nothing},
        {Op_return_imm, &CodeGen::Handle_return_imm},
        {Op_return_reg, &CodeGen::Handle_return_reg},
        {Op_increment_iv, &CodeGen::Handle_increment_iv},
        {Op_decrement_iv, &CodeGen::Handle_decrement_iv},
        {Op_neg_reg, &CodeGen::Handle_neg_reg},
    };
    for (IR ir : this->irs->content) {
        auto mark = this->irs->marks.find(count);
        if (mark != this->irs->marks.end()) {
            this->append("L" + std::to_string(mark->second) + ":");
        }
        auto it = handlers.find(ir.op);
        if (it != handlers.end()) {
            (this->*(it->second))(ir);
        }
        count ++;
    }
}
