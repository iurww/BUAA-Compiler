//
// Created by wwr on 2023/11/5.
//

#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H

#include <string>
#include <unordered_map>

class Type;

class Initial;

class Value;

class Symbol {
public:
    std::string name;
    Type *type;
    Value *value;
    bool is_const;
    bool is_global;
    bool is_complete;
    Initial *init;

    Symbol(std::string name, Type *type, Value *value, bool is_const, bool is_global, bool is_compelete, Initial *init)
            :
            name(name), type(type), value(value), is_const(is_const), is_global(is_global), is_complete(is_compelete),
            init(init) {}
};

class SymbolTable {
public:
    std::unordered_map<std::string, Symbol *> symbol_map;
    SymbolTable *parent{nullptr};
    std::vector<SymbolTable *> next;

    SymbolTable() = default;

    SymbolTable(SymbolTable *parent) {
        parent->next.push_back(this);
        this->parent = parent;
    }

    bool is_root() {
        return parent == nullptr;
    }

    void add(Symbol *symbol) {
        symbol_map[symbol->name] = symbol;
    }

    Symbol *get(std::string &name) {
        auto it = symbol_map.find(name);
        if (it == symbol_map.end() && !is_root()) {
            return parent->get(name);
        }
        if (it == symbol_map.end()) return nullptr;
        return it->second;
    }
};

#endif //COMPILER_SYMBOL_H
