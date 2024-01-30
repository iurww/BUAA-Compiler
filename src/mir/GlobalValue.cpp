//
// Created by wwr on 2023/11/7.
//

#include "../../include/mir/GlobalValue.h"
#include "../../include/mir/Initial.h"
#include "../../include/mir/Type.h"

GlobalValue::GlobalValue(std::string name, Type *type, Initial *init, bool is_const) :
        Value(type), init(init), is_const(is_const) {
    this->name = GLOBAL_PREFIX + name;
}

GlobalValue::operator std::string() const {
    return name + " = " + "dso_local " + (is_const ? "constant " : "global ") + std::string(*init) + "\n";
}
