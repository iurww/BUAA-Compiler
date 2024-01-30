//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_VALUE_H
#define COMPILER_VALUE_H

#include <string>

class Type;

class Value {
public:
    std::string name;
    Type *type;
    int id;

    static int VALUE_CNT;

    static std::string GLOBAL_PREFIX;
    static std::string LOCAL_PREFIX;
    static std::string GLOBAL_NAME_PREFIX;
    static std::string LOCAL_NAME_PREFIX;
    static std::string FPARAM_NAME_PREFIX;
    static std::string BB_NAME_PREFIX;

    Value(Type *type) : type(type) {};

    virtual ~Value() = default;

    virtual explicit operator std::string() const {
        return "value";
    }
};


#endif //COMPILER_VALUE_H
