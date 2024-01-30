//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_IRMODULE_H
#define COMPILER_IRMODULE_H

#include <unordered_map>
#include <string>
#include <vector>

class GlobalValue;

class Function;

class IRModule {
public:
    static IRModule *IR_MODULE;
    static bool is_external_func;

    std::unordered_map<std::string, GlobalValue *> globals;
    std::unordered_map<std::string, Function *> functions;
    std::vector<Function *> external_functions;

    IRModule();

    void add_global(GlobalValue *gv);

    void add_function(Function *func);

    friend std::ostream &operator<<(std::ostream &os, const IRModule &ir);

};

namespace EXTERN_FUNC {
    Function *new_getint();

    Function *new_putint();

    Function *new_putch();
}

#endif //COMPILER_IRMODULE_H
