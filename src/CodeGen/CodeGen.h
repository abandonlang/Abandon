#ifndef _CODEGEN_H_
#define _CODEGEN_H_
#include <sstream>
#include <string>
#include <utility>
#include "../IR/IR.h"
#include "../Symbol/Symbol.h"
#include "../Literal/Literal.h"
#include "../env_config/config.h"

struct FuncData {
    int allocate;
    std::string name;
    std::stringstream code;
};

class CodeGen {
    private:
    std::vector<FuncData> _output;
    void append(std::string ins);
    IRs * irs;
    Symbol * symbol;
    Literal literal;

    std::string getReg(bool isLow8, int n);
    inline std::string getReg(bool isLow8, Value reg);
    inline std::string getReg(int n);
    std::string getReg(const Value & reg);

    void Handle_newFunction_iv(const IR & ir);
    void Handle_endFunction(const IR & ir);
    void Handle_sign_defineVariable_type_iv(const IR & ir);
    void Handle_mov_iv_imm(const IR & ir);
    void Handle_xxx_reg_reg(const IR & ir);
    void Handle_xxxsd_reg_reg(const IR & ir);
    void Handle_idiv_val(const IR & ir);
    void Handle_cqo(const IR & ir);
    void Handle_load_imm_reg(const IR & ir);
    void Handle_load_iv_reg(const IR & ir);
    void Handle_load_mem_reg(const IR & ir);
    void Handle_store_iv_reg(const IR & ir);
    void Handle_jump_addr(const IR & ir);
    void Handle_jumpIf_addr_reg(const IR & ir);
    void Handle_jumpIfNot_addr_reg(const IR & ir);
    void Handle_compare_reg_reg(const IR & ir);
    void Handle_push_imm(const IR & ir);
    void Handle_push_iv(const IR & ir);
    void Handle_push_reg(const IR & ir);
    void Handle_pop_reg(const IR & ir);
    void Handle_pop_iv(const IR & ir);
    void Handle_call_if(const IR & ir);
    void Handle_return_with_nothing(const IR & ir);
    void Handle_return_imm(const IR & ir);
    void Handle_return_reg(const IR & ir);
    void Handle_increment_iv(const IR & ir);
    void Handle_decrement_iv(const IR & ir);
    void Handle_neg_reg(const IR & ir);
    
    public:
    CodeGen(IRs * irs, Symbol * symbol);
    void generate();
    std::string get_output();
};

#endif
