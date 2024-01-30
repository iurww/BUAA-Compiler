//
// Created by wwr on 2023/11/7.
//

#include "../../include/mir/IRModule.h"
#include "../../include/mir/GlobalValue.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/Type.h"
#include "../../include/util/init.h"
#include <iostream>
#include <string>

bool IRModule::is_external_func = true;

IRModule::IRModule() {
//    declare i32 @getint()
//    declare void @putint(i32)
//    declare void @putch(i32)
//    declare void @putstr(i8*)
    is_external_func = true;

    external_functions.push_back(EXTERN_FUNC::new_getint());
    external_functions.push_back(EXTERN_FUNC::new_putint());
    external_functions.push_back(EXTERN_FUNC::new_putch());

    is_external_func = false;
}

void IRModule::add_global(GlobalValue *gv) {
    globals[gv->name] = gv;
}

void IRModule::add_function(Function *func) {
    functions[func->name] = func;
}

std::ostream &operator<<(std::ostream &os, const IRModule &ir) {
    for (const auto &it: IRModule::IR_MODULE->external_functions) {
        os << "declare " << std::string(*it->ret_type) << " " << it->name;
        os << '(';
        for (int i = 0; i < it->params.size(); i++) {
            os << std::string(*it->params[i]->type);
            if (i < it->params.size() - 1) {
                os << ", ";
            }
        }
        os << ')' << std::endl;
    }

    os << std::endl;

    for (const auto &it: IRModule::IR_MODULE->globals) {
        os << std::string(*it.second);
    }

    os << std::endl;

    for (const auto &it: IRModule::IR_MODULE->functions) {
        os << *it.second << std::endl;
    }

    return os;
}

Function *EXTERN_FUNC::new_getint() {
    return new Function("getint",
                        new FuncType(new BasicType(TypeTag::int32_type)),
                        new BasicType(TypeTag::int32_type));
}

Function *EXTERN_FUNC::new_putint() {
    Function *PUT_INT = new Function("putint", new FuncType(new BasicType(TypeTag::void_type)),
                                     new BasicType(TypeTag::void_type));
    PUT_INT->add_param(new Function::Param(new BasicType(TypeTag::int32_type)));
    return PUT_INT;
}

Function *EXTERN_FUNC::new_putch() {
    Function *PUT_CH = new Function("putch", new FuncType(new BasicType(TypeTag::void_type)),
                                    new BasicType(TypeTag::void_type));
    PUT_CH->add_param(new Function::Param(new BasicType(TypeTag::int32_type)));
    return PUT_CH;
}

