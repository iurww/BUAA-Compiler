//
// Created by wwr on 2023/11/7.
//

#include "../../include/mir/Constant.h"
#include "../../include/mir/Type.h"

int ConstantInt::constant_id;

ConstantInt::ConstantInt(int val) : Value(new BasicType(TypeTag::int32_type)) {
    this->val = val;
    name = std::to_string(val);
}

std::string ConstantInt::get_descriptor() {
    return std::string(*type) + " " + name;
}

ConstantInt::operator std::string() const {
    return name;
}