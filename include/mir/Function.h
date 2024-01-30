//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_FUNCTION_H
#define COMPILER_FUNCTION_H

#include <vector>
#include "Value.h"

class LLVMInstr;

class BasicBlock;

class Function;

class Function : public Value {
public:
    class Param : public Value {
    public:
        Param(Type *type);

        ~Param() override = default;

        virtual explicit operator std::string() const override;
    };

    std::vector<Param *> params;
    std::vector<BasicBlock *> blocks;
    Type *ret_type;



    Function(std::string func_name, Type *value_type, Type *ret_type);

    ~Function() = default;

    void add_param(Param *param);

    void add_block_back(BasicBlock *bb);

    virtual explicit operator std::string() const override;

    friend std::ostream &operator<<(std::ostream &os, const Function &func);

};


#endif //COMPILER_FUNCTION_H
