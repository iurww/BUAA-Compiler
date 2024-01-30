//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_CONSTANT_H
#define COMPILER_CONSTANT_H

#include "Value.h"

class ConstantInt : public Value {
public:
    static int constant_id;
    int val;


    ConstantInt(int val);

    ~ConstantInt() = default;

    std::string get_descriptor();

    virtual explicit operator std::string() const override;
};


#endif //COMPILER_CONSTANT_H
