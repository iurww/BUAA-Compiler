//
// Created by wwr on 2023/11/7.
//

#include <iostream>
#include "../../include/mir/Initial.h"
#include "../../include/mir/Type.h"
#include "Constant.h"

Initial::Initial(Type *type) : type(type) {

}


InitialVal::InitialVal(Type *type, Value *value) : Initial(type), value(value) {

}

InitialVal::operator std::string() const {
    return std::string(*value->type) + " " + std::string(*value);
}

bool InitialVal::is_zero() {
    return ((ConstantInt *) value)->val == 0;
}


InitialArray::InitialArray(Type *type) : Initial(type) {

}

void InitialArray::add(Initial *init) {
    if (auto p1 = dynamic_cast<InitialVal *> (init)) {
        init_val.push_back(p1->value);
    } else if (auto p2 = dynamic_cast<InitialArray *> (init)) {
        init_val.insert(init_val.end(), p2->init_val.begin(), p2->init_val.end());
    } else if (auto p3 = dynamic_cast<InitialExp *> (init)) {
        init_val.push_back(p3->result);
    }
    inits.push_back(init);
}

int InitialArray::size() {
    return inits.size();
}

InitialArray::operator std::string() const {
    std::string s = "";
    for (int i = 0; i < inits.size(); ++i) {
        if (inits[i]->is_zero() && inits[i]->type->is_array_type()) {
            s.append(std::string(*inits[i]->type)).append(" zeroinitializer");
        } else {
            s.append(std::string(*inits[i]));
        }
        if (i != inits.size() - 1) {
            s.append(", ");
        }
    }
    return std::string(*type) + " [" + s + "]";
}

bool InitialArray::is_zero() {
    for (Initial *initial: inits) {
        if (!initial->is_zero())
            return false;
    }
    return true;
}

Value *InitialArray::get(std::vector<int> &index) {
    int res = 1;
    for (int idx: index) {
        res *= idx;
    }
    return init_val[res - 1];
}


InitialExp::InitialExp(Type *type, Value *result) : Initial(type), result(result) {

}

InitialExp::operator std::string() const {
    return std::string(*result);
}


InitialZero::InitialZero(Type *type) : Initial(type) {

}

InitialZero::operator std::string() const {
    return std::string(*type) + " zeroinitializer";
}