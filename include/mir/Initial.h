//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_INITIAL_H
#define COMPILER_INITIAL_H

#include <vector>
#include "Value.h"

class Initial {
public:
    Type *type;

    Initial(Type *type);

    virtual ~Initial() = default;

    virtual explicit operator std::string() const = 0;

    virtual bool is_zero() {
        return false;
    }
};

class InitialVal : public Initial {
public:
    Value *value;

    InitialVal(Type *type, Value *value);

    virtual ~InitialVal() override = default;

    virtual explicit operator std::string() const override;

    bool is_zero() override;
};

class InitialArray : public Initial {
public:
    std::vector<Initial *> inits;
    std::vector<Value *> init_val;

    InitialArray(Type *type);

    void add(Initial *init);

    int size();

    virtual ~InitialArray() override = default;

    virtual explicit operator std::string() const override;

    bool is_zero() override;

    Value *get(std::vector<int> &index);
};

class InitialExp : public Initial {
public:
    Value *result;

    InitialExp(Type *type, Value *result);

    virtual ~InitialExp() override = default;

    virtual explicit operator std::string() const override;
};

class InitialZero : public Initial {
public:

    InitialZero(Type *type);

    virtual ~InitialZero() override = default;

    virtual explicit operator std::string() const override;
};

#endif //COMPILER_INITIAL_H
