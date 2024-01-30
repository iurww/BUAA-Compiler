//
// Created by wwr on 2023/11/7.
//

#include "../../include/mir/Type.h"
#include "../../include/frontend/Token.h"

Type::Type(TypeTag type) {
    this->type_tag = type;
}

Type::Type(Token *token) {
    if (token->is_of(TokenType::INTTK)) {
        this->type_tag = TypeTag::int32_type;
    } else if (token->is_of(TokenType::VOIDTK)) {
        this->type_tag = TypeTag::void_type;
    }
}

bool Type::is_basic_type() const {
    return type_tag <= TypeTag::int32_type;
}

bool Type::is_void_type() const {
    return type_tag == TypeTag::void_type;
}

bool Type::is_array_type() const {
    return type_tag == TypeTag::array_type;
}

bool Type::is_pointer_type() const {
    return type_tag == TypeTag::pointer_type;
}

bool Type::operator==(const Type &other) const {
    if (type_tag != other.type_tag) return false;
    if (is_basic_type()) {
        return dynamic_cast<const BasicType &>(*this) == dynamic_cast<const BasicType &>(other);
    } else if (is_array_type()) {
        return dynamic_cast<const ArrayType &>(*this) == dynamic_cast<const ArrayType &>(other);
    } else if (is_pointer_type()) {
        return dynamic_cast<const PointerType &>(*this) == dynamic_cast<const PointerType &>(other);
    } else {
        return true;
    }
}

bool Type::operator!=(const Type &other) const {
    return !(*this == other);
}

BasicType::BasicType(TypeTag type) : Type(type) {

}

BasicType::BasicType(Token *token) : Type(token) {

}

bool BasicType::operator==(const Type &other) const {
    return Type::operator==(other);
}

bool BasicType::operator!=(const Type &other) const {
    return Type::operator!=(other);
}

bool BasicType::operator==(const BasicType &other) const {
    return type_tag == other.type_tag;
}

bool BasicType::operator!=(const BasicType &other) const {
    return type_tag != other.type_tag;
}


ArrayType::ArrayType(int size, Type *base_type) : Type(TypeTag::array_type) {
    this->size = size;
    this->base_type = base_type;
    dims.push_back(size);
    if (base_type->is_array_type()) {
        ArrayType *array_type = (ArrayType *) base_type;
        dims.insert(dims.end(), array_type->dims.begin(), array_type->dims.end());
    }
}

bool ArrayType::operator==(const Type &other) const {
    return Type::operator==(other);
}

bool ArrayType::operator!=(const Type &other) const {
    return Type::operator!=(other);
}

bool ArrayType::operator==(const ArrayType &other) const {
    if (size != other.size) return false;
    if (base_type->is_array_type() && other.base_type->is_array_type()) {
        return dynamic_cast<const ArrayType &>(*base_type) == dynamic_cast<const ArrayType &>(*other.base_type);
    } else if (base_type->is_basic_type() && other.base_type->is_basic_type()) {
        return dynamic_cast<const BasicType &>(*base_type) == dynamic_cast<const BasicType &>(*other.base_type);
    }
    return false;
}

bool ArrayType::operator!=(const ArrayType &other) const {
    return !(*this == other);
}


BBType::BBType() : Type(TypeTag::bb_type) {

}

bool BBType::operator==(const Type &other) const {
    return other.type_tag == TypeTag::bb_type;
}

bool BBType::operator!=(const Type &other) const {
    return other.type_tag != TypeTag::bb_type;
}


FuncType::FuncType(Type *ret_type) : Type(TypeTag::func_type), ret_type(ret_type) {
    fparam_cnt = 0;
}

void FuncType::add_param_type(Type *type) {
    fparam_type.push_back(type);
    fparam_cnt++;
}

bool FuncType::operator==(const Type &other) const {
    if (other.type_tag != TypeTag::func_type) return false;
    return operator==(dynamic_cast<const FuncType &>(other));
}

bool FuncType::operator!=(const Type &other) const {
    if (other.type_tag != TypeTag::func_type) return true;
    return operator!=(dynamic_cast<const FuncType &>(other));
}

bool FuncType::operator==(const FuncType &other) const {
    if (*ret_type != *other.ret_type) return false;
    if (this->fparam_cnt != other.fparam_cnt) return false;
    for (int i = 0; i < fparam_type.size(); i++) {
        if (*fparam_type[i] != *other.fparam_type[i])
            return false;
    }
    return true;
}

bool FuncType::operator!=(const FuncType &other) const {
    return !(*this == other);
}


PointerType::PointerType(Type *ref_type) : Type(TypeTag::pointer_type), ref_type(ref_type) {

}

bool PointerType::operator==(const Type &other) const {
    if (!other.is_pointer_type()) return false;
    return operator==(dynamic_cast<const PointerType &>(other));
}

bool PointerType::operator!=(const Type &other) const {
    if (!other.is_pointer_type()) return true;
    return operator!=(dynamic_cast<const PointerType &>(other));
}

bool PointerType::operator==(const PointerType &other) const {
    return *ref_type == *other.ref_type;
}

bool PointerType::operator!=(const PointerType &other) const {
    return *ref_type != *other.ref_type;
}