#include "Symbol.h"
#include "../SayError/SayError.h"
#include <stdexcept>
#include <string>

Symbol::Symbol() {
    this->new_scope();
}
int Symbol::assign_memory(int addr, int rest, int size, int & used) {
    int frame_offset;
    used = size;
    frame_offset = addr + (rest - size);
    if (size == 2 || size == 4 || size == 8) {
        int extra = frame_offset % size;
        if (extra != 0) {
            frame_offset -= extra;
            used += extra;
        }
    }
    return frame_offset;
}

void Symbol::insert_variable(std::string name, TypeType var_type) {
    SymbolValue sv;
    ScopeValue & scope = this->table.back();
    int size;
    sv.isVariable = true;
    sv.type = var_type;
    switch (var_type) {
        case TYPE_CHAR: size = CHAR_SIZE; break;
        case TYPE_INT: size = INT_SIZE; break;
        case TYPE_FLOAT: size = FLOAT_SIZE; break;
        default: break; 
        // TODO
    }
    /* find suitable memory address */
    auto it = scope.memory_unused.begin();
    for (; it != scope.memory_unused.end();  ++ it) {
        if (it->second >= size) {
            int used;
            sv.frame_offset = this->assign_memory(it->first, it->second, size, used);
            it->second -= used;
            if (it->second == 0) {
                scope.memory_unused.erase(it);
            }
            break;
        }
    }
    // if cannot find suitable memory to stay
    if (it == scope.memory_unused.end()) {
        if (size == 8) {
            sv.frame_offset = scope.i;
        } else {
            int used;
            sv.frame_offset = this->assign_memory(scope.i, 8, size, used);
            scope.memory_unused.push_back({scope.i, (8-used)});
        }
        scope.i -= 8;
    }
    //
    sv.size = size;
    scope.map.insert({name, sv});
}

SymbolValue Symbol::get_variable(std::string name) {
    SymbolValue sv = this->get(name);
    if (sv.isExist && !sv.isVariable) {
        sv.isExist = false;
        return sv;
    }
    return sv;
}
SymbolValue Symbol::get_variable(Value val) {
    if (val.isVariable()) {
        return this->get_variable(val.getVariable().content);
    }
    return {.isExist = false,
            .isVariable = false,// not important
            .type = TYPE_UNKNOWN,
            .frame_offset = 0,
            .size = 0,
            .args = {},
    };
}
std::string Symbol::get_variable_mem(Value val) {
    if (!val.isVariable()) {
        std::runtime_error("Symbol::get_variable_mem(Value val)");
    }
    return this->get_variable_mem(val.getVariable().content);
}

std::string Symbol::get_variable_mem(std::string name) {
    SymbolValue sv = this->get_variable(name);
    std::string ret;
    if (!sv.isExist) {
        sayError("`" + name + "` does not exist.\n");
    }
    if (!sv.isVariable) {
        sayError("`" + name + "` is a function.\n");
    }
    switch (sv.size) {
        case 1: ret = "BYTE "; break;
        case 2: ret = "WORD "; break;
        case 4: ret = "DWORD "; break;
        case 8: ret = "QWORD "; break;
    }
    if (sv.frame_offset > 0) {
        ret += "[rbp+" + std::to_string(sv.frame_offset) + "]";
    } else if (sv.frame_offset < 0) {
        ret += "[rbp" + std::to_string(sv.frame_offset) + "]";
    } else if (sv.frame_offset == 0) {
        ret += "[rbp]";
    }
    return ret;
}


void Symbol::insert_function(std::string name, SymbolValue sv) {
    ScopeValue & scope = this->table[0];
    scope.map.insert({name, sv});
}
SymbolValue Symbol::get(std::string name) {
    for (auto it = this->table.rbegin();  it != this->table.rend();  ++ it) {
        ScopeValue & scope = *it;
        auto result = scope.map.find(name);
        if (result != scope.map.end()) {
            return result->second;
        }
    }
    // Cannot find out
    SymbolValue sv;
    sv.isExist = false;
    return sv;
}

void Symbol::new_scope() {
    this->table.push_back({
        .map = {},
        .i = (-8),
        .memory_unused = {}
    });
}
int Symbol::exit_scope() {
    int i = this->table.back().i;
    this->table.pop_back();
    return i;
}
