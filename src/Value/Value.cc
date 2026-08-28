#include "Value.h"
#include <string>

std::string TypeTypeToString(TypeType t) {
    switch (t) {
        case TYPE_UNKNOWN: return "UNKNOWN";
        case TYPE_VOID: return "void";
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
    }
    return "UNKNOWN";
}

TypeType getImmediateType(const Immediate& imm) {
    return imm.type;
}

Immediate makeImmediate(TypeType type, std::string s) {
    Immediate imm;
    imm.type = type;
    imm.content = s;
    return imm;
}

Immediate makeImmediate(int i) {
    return makeImmediate(TYPE_INT,  std::to_string(i));
}

Variable makeVariable(std::string content) {
    Variable iv;
    iv.content = content;
    return iv;
}

Value::Value() {}
Value::Value(const Immediate & imm) {
    this->data_ = imm;
}
Value::Value(const std::string & iv_name) {
    this->data_ = makeVariable(iv_name);
}
Value::Value(const Variable & iv) {
    this->data_ = iv;
}
Value::Value(char * reg) {
    this->data_ = reg;
}
Value::Value(int offset) {
    this->data_ = offset;
}
Value::Value(const TypeType & type) {
    this->data_ = type;
}
Value::Value(const SpecialMark & spm) {
    this->data_ = spm;
}
Value& Value::operator=(const Immediate & o) {
    this->data_ = o;
    return *this;
}

Value& Value::operator=(const Variable & o) {
    this->data_ = o;
    return *this;
}

Value& Value::operator=(const std::string & o) {
    this->data_ = makeVariable(o);
    return *this;
}
bool Value::operator==(const Value & o) {
    if (this->data_.index() != o.data_.index()) {
        return false;
    }
    if (std::holds_alternative<Immediate>(data_)) {
        auto me = std::get<Immediate>(data_);
        auto you = std::get<Immediate>(o.data_);
        return me.type == you.type && me.content == you.content;
    }
    if (std::holds_alternative<Variable>(data_)) {
        auto me = std::get<Variable>(data_);
        auto you = std::get<Variable>(o.data_);
        return me.content == you.content;
    }
    if (std::holds_alternative<char*>(data_)) {
        auto me = std::get<char*>(data_);
        auto you = std::get<char*>(o.data_);
        return me == you;
        // the address has the fixed value
    }
    if (std::holds_alternative<TypeType>(data_)) {
        auto me = std::get<TypeType>(data_);
        auto you = std::get<TypeType>(o.data_);
        return me == you;
    }
    return false;
}
bool Value::operator!=(const Value & o) {
    return !this->operator==(o);
}
bool Value::isVariable() const {
    return std::holds_alternative<Variable>(data_);
}
bool Value::isImmediate() const {
    return std::holds_alternative<Immediate>(data_);
}
bool Value::isReg() const {
    return std::holds_alternative<char*>(data_);
}
bool Value::isParaHead() const {
    if (!std::holds_alternative<SpecialMark>(data_)) {
        return false;
    }
    return (FUNCTION_CALL_PARA_HEAD == std::get<SpecialMark>(data_));
}
const Immediate& Value::getImmediate() const {
    return std::get<Immediate>(data_);
}
const Variable& Value::getVariable() const {
    return std::get<Variable>(data_);
}
char* Value::getReg() const {
    return std::get<char*>(data_);
}
int Value::getMem() const {
    return std::get<int>(data_);
}
const TypeType& Value::getType() const {
    return std::get<TypeType>(data_);
}

bool Value::isRegFloat() const {
    if (!this->isReg()) return false;
    char * reg_addr = this->getReg();
    return reg_addr[0] == 'x' && reg_addr[1] == 'm' && reg_addr[2] == 'm';
}

#ifdef DEBUG
#include <format>
std::string Value::toString() const {
    if (std::holds_alternative<Immediate>(data_)) {
        auto me = std::get<Immediate>(data_);
        return me.content;
    }
    if (std::holds_alternative<Variable>(data_)) {
        auto me = std::get<Variable>(data_);
        return me.content;
    }
    if (std::holds_alternative<char*>(data_)) {
        auto me = std::get<char*>(data_);
        return me;
    }
    if (std::holds_alternative<int>(data_)) {
        auto me = std::get<int>(data_);
        return std::format("[rsp+{}]", me);
    }
    return std::string();
}
#endif
