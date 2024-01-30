//
// Created by wwr on 2023/11/7.
//

#include <iostream>
#include "../../include/mir/LLVMInstr.h"
#include "../../include/mir/BasicBlock.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/Type.h"
#include "../../include/mir/Value.h"
#include "../../include/mir/Use.h"
#include "Constant.h"


LLVMInstr::LLVMInstr(Type *type, BasicBlock *cur_bb) : Value(type) {
    bb = cur_bb;
    if (!bb->is_closed)
        bb->add_instr_back(this);
}

void LLVMInstr::set_use(Value *value) {
    Use *use = new Use(this, value);
    use_list.push_back(use);
    used.push_back(value);
}


LLVM::Alu::Alu(Type *type, AluOp op, Value *v1, Value *v2, BasicBlock *cur_bb) : LLVMInstr(type, cur_bb) {
    this->op = op;
    src[0] = v1;
    src[1] = v2;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(v1);
    set_use(v2);
}

LLVM::Alu::operator std::string() const {
    return name + " = " + LLVM::Alu::get_alu_op_name(op) + " " +
           std::string(*type) + " " + src[0]->name + ", " + src[1]->name;
}

std::string LLVM::Alu::get_alu_op_name(LLVM::Alu::AluOp op) const {
    static std::string alu_op_name[10] = {
            "add", "sub", "mul", "sdiv", "srem"
    };
    return alu_op_name[(int) op];
}

LLVM::Icmp::Icmp(IcmpOp op, Value *v1, Value *v2, BasicBlock *cur_bb) : LLVMInstr(new BasicType(TypeTag::int1_type),
                                                                                  cur_bb) {
    this->op = op;
    src[0] = v1;
    src[1] = v2;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(v1);
    set_use(v2);
}

std::string LLVM::Icmp::get_icmp_op_name(LLVM::Icmp::IcmpOp op) const {
    static std::string icmp_op_name[10] = {
            "slt", "sle", "sgt", "sge", "eq", "ne"
    };
    return icmp_op_name[(int) op];
}

LLVM::Icmp::operator std::string() const {
    return name + " = " + "icmp " + LLVM::Icmp::get_icmp_op_name(op) + " " +
           std::string(*src[0]->type) + " " + src[0]->name + ", " + src[1]->name;
}


LLVM::Alloca::Alloca(Type *type, BasicBlock *cur_bb) : LLVMInstr(new PointerType(type), cur_bb) {
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    this->content_type = type;
}

LLVM::Alloca::operator std::string() const {
    return name + " = alloca " + std::string(*content_type);
}


LLVM::Load::Load(Value *src, BasicBlock *cur_bb) : LLVMInstr(((PointerType *) src->type)->ref_type, cur_bb) {
    this->src = src;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(src);
}

LLVM::Load::operator std::string() const {
    return name + " = load " + std::string(*type) + ", " +
           std::string(*src->type) + " " + src->name;
}


LLVM::Store::Store(Value *src, Value *dst, BasicBlock *cur_bb) :
        LLVMInstr(new BasicType(TypeTag::void_type), cur_bb) {
    this->src = src;
    this->dst = dst;
    this->name = dst->name;
    set_use(src);
    set_use(dst);
}

LLVM::Store::operator std::string() const {
    return "store " + std::string(*src->type) + " " + src->name + ", " +
           std::string(*dst->type) + " " + dst->name;
}


LLVM::Return::Return(BasicBlock *cur_bb) : LLVMInstr(new BasicType(TypeTag::void_type), cur_bb) {
    ret_value = nullptr;
    cur_bb->is_closed = true;
}

LLVM::Return::Return(Value *ret, BasicBlock *cur_bb) : LLVMInstr(new BasicType(TypeTag::void_type), cur_bb) {
    ret_value = ret;
    cur_bb->is_closed = true;
    set_use(ret);
}

LLVM::Return::operator std::string() const {
    if (ret_value == nullptr) {
        return "ret void";
    } else {
        return "ret " + std::string(*ret_value->type) + " " + ret_value->name;
    }
}


LLVM::Call::Call(Function *func, Type *ret_type, std::vector<Value *> rparams, BasicBlock *cur_bb) : LLVMInstr(
        ret_type, cur_bb) {
    this->func = func;
    this->rparams = std::move(rparams);
    if (ret_type->is_void_type()) {
        this->name = "";
        is_void = true;
    } else {
        id = VALUE_CNT;
        name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
        is_void = false;
    }
    for (Value *param : rparams) {
        set_use(param);
    }
}

LLVM::Call::Call(Function *func, Type *ret_type, BasicBlock *cur_bb) : LLVMInstr(
        ret_type, cur_bb) {
    this->func = func;
    if (ret_type->is_void_type()) {
        this->name = "";
        is_void = true;
    } else {
        id = VALUE_CNT;
        name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
        is_void = false;
    }
}


LLVM::Call::operator std::string() const {
    std::string s = "";
    if (!is_void) {
        s.append(this->name).append(" = ");
    }
    s.append("call ").append(std::string(*func->ret_type)).append(" ")
            .append(func->name).append("(");
    for (int i = 0; i < rparams.size(); i++) {
        s.append(std::string(*rparams[i]->type)).append(" ").append(rparams[i]->name);
        if (i < rparams.size() - 1)
            s.append(", ");
    }
    s.append(")");

    return s;
}


LLVM::Jump::Jump(BasicBlock *dst, BasicBlock *cur_bb) : LLVMInstr(new BasicType(TypeTag::void_type), cur_bb) {
    this->dst = dst;
    this->name = "";
    cur_bb->is_closed = true;
    set_use(dst);
}

LLVM::Jump::operator std::string() const {
    return "br label %" + dst->name;
}


LLVM::Branch::Branch(Value *cond, BasicBlock *true_block, BasicBlock *false_block, BasicBlock *cur_bb) : LLVMInstr(
        new BasicType(TypeTag::void_type), cur_bb) {
    this->cond = cond;
    this->true_block = true_block;
    this->false_block = false_block;
    this->name = "";
    cur_bb->is_closed = true;
    set_use(cond);
    set_use(true_block);
    set_use(false_block);
}

LLVM::Branch::operator std::string() const {
    std::string s = "";
    s.append("br i1 ").append(cond->name).append(", ");
    s.append("label %").append(true_block->name).append(", ");
    s.append("label %").append(false_block->name);
    return s;
}


LLVM::GetElementPtr::GetElementPtr(Type *type, Value *src, std::vector<int> dims, BasicBlock *cur_bb) : LLVMInstr(
        type, cur_bb) {
    this->src = src;
    this->const_dims = true;
    this->dims = dims;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(src);
    for (int dim : dims) {
        set_use(new ConstantInt(dim));
    }
}

LLVM::GetElementPtr::GetElementPtr(Type *type, Value *src, std::vector<Value *> dims, BasicBlock *cur_bb)
        : LLVMInstr(
        type, cur_bb) {
    this->src = src;
    this->const_dims = false;
    this->dims_value = dims;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(src);
    for (Value *v : dims) {
        set_use(v);
    }
}

LLVM::GetElementPtr::operator std::string() const {
    std::string s = "";
    Type *ref_type = ((PointerType *) src->type)->ref_type;
    s.append(name).append(" = getelementptr ").append(std::string(*ref_type));
    s.append(", ").append(std::string(*src->type)).append(" ").append(src->name);
    if (dims.size() > 0) {
        for (int x: dims) {
            s.append(", i32 ").append(std::to_string(x));
        }
    } else {
        for (Value *v: dims_value) {
            s.append(", i32 ").append(v->name);
        }
    }

    return s;
}


LLVM::Zext::Zext(Value *src, Type *dst_type, BasicBlock *cur_bb) : LLVMInstr(dst_type, cur_bb) {
    this->src = src;
    id = VALUE_CNT;
    name = LOCAL_PREFIX + LOCAL_NAME_PREFIX + std::to_string(VALUE_CNT++);
    set_use(src);
}

LLVM::Zext::operator std::string() const {
    return name + " = zext " + std::string(*src->type) +
           " " + src->name + " to " + std::string(*type);
}
