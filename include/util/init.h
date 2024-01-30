//
// Created by wwr on 2023/11/17.
//

#ifndef COMPILER_INIT_H
#define COMPILER_INIT_H

#include "../mir/Value.h"

int Value::VALUE_CNT = 0;
std::string Value::GLOBAL_PREFIX("@");
std::string Value::LOCAL_PREFIX = "%";
std::string Value::LOCAL_NAME_PREFIX = "v";
std::string Value::FPARAM_NAME_PREFIX = "f";
std::string Value::BB_NAME_PREFIX = "b";


#include "../mir/IRModule.h"

IRModule *IRModule::IR_MODULE = new IRModule();


#include "../backend/CodeGen.h"


CodeGen *CodeGen::CODE_GEN = new CodeGen();


#endif //COMPILER_INIT_H
