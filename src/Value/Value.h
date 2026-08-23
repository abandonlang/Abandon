#ifndef _VALUE_H_
#define _VALUE_H_
#include <string>
#include <variant>
enum TypeType {
    TYPE_UNKNOWN, 
    TYPE_INT, 
    TYPE_CHAR, 
    TYPE_FLOAT, 
    TYPE_STRING,
    TYPE_VOID,
};

std::string TypeTypeToString(TypeType t);

struct Immediate {
    TypeType type;
    std::string content;
};

TypeType getImmediateType(const Immediate& imm) ;

struct IdVariable {
    std::string content;
};

Immediate makeImmediate(TypeType type, std::string s);
Immediate makeImmediate(int i);
IdVariable makeIdVariable(std::string content);

enum SpecialMark {
    FUNCTION_CALL_PARA_HEAD,
};

// for jump op
struct Address {
    int line;
    Address() = default;
    Address(int n) : line (n) {}
};

class Value {
    private:
    std::variant<
        std::monostate, 
        Immediate,    // immediate
        IdVariable,   // iv, if
        int,          // reg number
        TypeType,     // tags of a type
        SpecialMark
    > data_;
    public:
    Value();
    Value(const Value & o) = default;
    Value(const Immediate & imm);
    Value(const std::string & iv_name);
    Value(const IdVariable & iv);
    Value(const int & reg);
    Value(const TypeType & type);
    Value(const SpecialMark & spm);
    ~Value() = default;
    Value& operator=(const Value & o) = default;
    Value& operator=(const Immediate & o);
    Value& operator=(const IdVariable & o);
    Value& operator=(const std::string & o);
    bool operator==(const Value & o);
    bool operator!=(const Value & o);

    bool isVariable() const;
    bool isImmediate() const;
    bool isReg() const;
    bool isParaHead() const;
    const Immediate& getImmediate() const;
    const IdVariable& getIdVariable() const;
    const int& getReg() const;
    const TypeType& getType() const;
    #ifdef DEBUG
    std::string toString() const ;
    #endif
};

#endif
