//
// Created by wwr on 2023/11/5.
//

#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include <vector>
#include <string>

class Token;

enum class TypeTag {
    int1_type = 0,
    int32_type = 1,
    void_type = 2,
    array_type = 3,
    bb_type = 4,
    func_type = 5,
    pointer_type = 6,
};

class Type {
public:
    TypeTag type_tag = TypeTag::void_type;

    explicit Type(TypeTag type);

    explicit Type(Token *token);

    virtual ~Type() = default;

    virtual explicit operator std::string() const {
        return "base_type";
    }

    virtual bool operator==(const Type &other) const;

    virtual bool operator!=(const Type &other) const;

    bool is_basic_type() const;

    bool is_void_type() const;

    bool is_array_type() const;

    bool is_pointer_type() const;

    virtual int get_full_size() const {
        return 0;
    }
};

class BasicType : public Type {
public:

    explicit BasicType(TypeTag type);

    explicit BasicType(Token *token);

    ~BasicType() override = default;

    virtual explicit operator std::string() const override {
        if (type_tag == TypeTag::int1_type) {
            return "i1";
        } else if (type_tag == TypeTag::int32_type) {
            return "i32";
        } else {
            return "void";
        }
    }

    bool operator==(const Type &other) const override;

    bool operator!=(const Type &other) const override;

    bool operator==(const BasicType &other) const;

    bool operator!=(const BasicType &other) const;

    virtual int get_full_size() const override {
        if (type_tag == TypeTag::void_type) {
            return 0;
        } else {
            return 1;
        }
    }
};

class ArrayType : public Type {
public:
    int size;
    Type *base_type;
    std::vector<int> dims;

    explicit ArrayType(int size, Type *base_type);

    ~ArrayType() override = default;

    virtual explicit operator std::string() const override {
        return "[" + std::to_string(size) + " x " + std::string(*base_type) + "]";
    }

    bool operator==(const Type &other) const override;

    bool operator!=(const Type &other) const override;

    bool operator==(const ArrayType &other) const;

    bool operator!=(const ArrayType &other) const;

    virtual int get_full_size() const override {
        int res = 1;
        for (int x : dims) res *= x;
        return res;
    }
};

class BBType : public Type {
public:

    explicit BBType();

    ~BBType() override = default;

    virtual explicit operator std::string() const override {
        return "b ";
    }

    bool operator==(const Type &other) const override;

    bool operator!=(const Type &other) const override;

    virtual int get_full_size() const override {
        return 0;
    }
};

class FuncType : public Type {
public:
    int fparam_cnt;
    std::vector<Type *> fparam_type;
    Type *ret_type;

    explicit FuncType(Type *ret_type);

    ~FuncType() override = default;

    void add_param_type(Type *type);

    bool operator==(const Type &other) const override;

    bool operator!=(const Type &other) const override;

    bool operator==(const FuncType &other) const;

    bool operator!=(const FuncType &other) const;

    virtual int get_full_size() const override {
        return 1;
    }
};

class PointerType : public Type {
public:
    Type *ref_type;

    explicit PointerType(Type *ref_type);

    ~PointerType() override = default;

    virtual explicit operator std::string() const override {
        return std::string(*ref_type) + "*";
    }

    bool operator==(const Type &other) const override;

    bool operator!=(const Type &other) const override;

    bool operator==(const PointerType &other) const;

    bool operator!=(const PointerType &other) const;

    virtual int get_full_size() const override {
        return 1;
    }
};

#endif //COMPILER_TYPE_H
