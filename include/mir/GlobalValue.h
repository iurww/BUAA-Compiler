//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_GLOBALVALUE_H
#define COMPILER_GLOBALVALUE_H


#include "Value.h"

class Initial;

class GlobalValue : public Value {
public:
    Initial *init;
    bool is_const;

    GlobalValue(std::string name, Type *type, Initial *init, bool is_const);

    ~GlobalValue() = default;

    virtual explicit operator std::string() const override;

};


#endif //COMPILER_GLOBALVALUE_H
