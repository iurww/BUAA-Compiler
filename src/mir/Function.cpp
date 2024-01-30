//
// Created by wwr on 2023/11/7.
//

#include <sstream>

#include "../../include/mir/Type.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/IRModule.h"
#include "../../include/mir/BasicBlock.h"

Function::Param::Param(Type *type) : Value(type) {
    if (IRModule::is_external_func) {
        name = "";
    } else {
        id = VALUE_CNT;
        name = LOCAL_PREFIX + FPARAM_NAME_PREFIX + std::to_string(VALUE_CNT++);
    }
}

Function::Param::operator std::string() const {
    return std::string(*type) + " " + name;
}

Function::Function(std::string func_name, Type *value_type, Type *ret_type) : Value(value_type), ret_type(ret_type) {
    name = GLOBAL_PREFIX + func_name;
}

void Function::add_param(Function::Param *param) {
    params.push_back(param);
}

void Function::add_block_back(BasicBlock *bb) {
    blocks.push_back(bb);
}

Function::operator std::string() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const Function &func) {
    std::ostringstream oss;
    for (int i = 0; i < func.params.size(); i++) {
        oss << std::string(*func.params[i]);
        if (i < func.params.size() - 1) {
            oss << ", ";
        }
    }

    std::string str;
    str.append("define dso_local ").append(std::string(*func.ret_type)).append(" ")
            .append(func.name).append("(").append(oss.str()).append(")").append(" {");
    os << str << std::endl;

    for (const auto it: func.blocks) {
        os << *it;
    }

    os << "}" << std::endl;
    return os;
}