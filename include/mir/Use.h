//
// Created by wwr on 2023/12/12.
//

#ifndef COMPILER_USE_H
#define COMPILER_USE_H


class Value;

class LLVMInstr;

class Use {
public:
    LLVMInstr * user{};
    Value *used{};

public:
    Use(LLVMInstr *user, Value *used) :
            user(user), used(used) {}

    ~Use() = default;
};


#endif //COMPILER_USE_H
