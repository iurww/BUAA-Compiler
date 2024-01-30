//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_LLVMINSTR_H
#define COMPILER_LLVMINSTR_H

#include <vector>
#include "../../include/mir/Value.h"
#include "../../include/mir/Use.h"

class BasicBlock;

class Function;

class Type;

class LLVMInstr : public Value {
public:
    BasicBlock *bb;
    std::vector<Use *> use_list;
    std::vector<Value *> used;

    LLVMInstr(Type *type, BasicBlock *cur_bb);

    void set_use(Value *value);

    virtual ~LLVMInstr() = default;

    virtual explicit operator std::string() const override {
        return "instruction";
    }
};

namespace LLVM {
    class Alu : public LLVMInstr {
    public:
        enum class AluOp {
            add,
            sub,
            mul,
            sdiv,
            srem
        };

        AluOp op;
        Value *src[2];

        Alu(Type *type, AluOp op, Value *v1, Value *v2, BasicBlock *cur_bb);

        std::string get_alu_op_name(AluOp op) const;

        ~Alu() override = default;

        virtual explicit operator std::string() const override;
    };

    class Icmp : public LLVMInstr {
    public:
        enum class IcmpOp {
            slt,
            sle,
            sgt,
            sge,
            eq,
            ne
        };

        IcmpOp op;
        Value *src[2];

        Icmp(IcmpOp op, Value *v1, Value *v2, BasicBlock *cur_bb);

        std::string get_icmp_op_name(IcmpOp op) const;

        ~Icmp() override = default;

        virtual explicit operator std::string() const override;
    };

    class Alloca : public LLVMInstr {
    public:
        Type *content_type;

        Alloca(Type *type, BasicBlock *cur_bb);

        ~Alloca() override = default;

        virtual explicit operator std::string() const override;
    };

    class Load : public LLVMInstr {
    public:
        Value *src;

        Load(Value *src, BasicBlock *cur_bb);

        ~Load() override = default;

        virtual explicit operator std::string() const override;
    };

    class Store : public LLVMInstr {
    public:
        Value *src;
        Value *dst;

        Store(Value *src, Value *dst, BasicBlock *cur_bb);

        ~Store() override = default;

        virtual explicit operator std::string() const override;
    };

    class Return : public LLVMInstr {
    public:
        Value *ret_value;

        Return(BasicBlock *cur_bb);

        Return(Value *ret, BasicBlock *cur_bb);

        ~Return() override = default;

        virtual explicit operator std::string() const override;
    };

    class Call : public LLVMInstr {
    public:
        Function *func;
        bool is_void;
        std::vector<Value *> rparams;

        Call(Function *func, Type *ret_type, std::vector<Value *> rparams, BasicBlock *cur_bb);

        Call(Function *func, Type *ret_type, BasicBlock *cur_bb);

        ~Call() override = default;

        virtual explicit operator std::string() const override;
    };

    class Jump : public LLVMInstr {
    public:
        BasicBlock *dst;

        Jump(BasicBlock *dst, BasicBlock *cur_bb);

        ~Jump() override = default;

        virtual explicit operator std::string() const override;
    };

    class Branch : public LLVMInstr {
    public:
        Value *cond;
        BasicBlock *true_block;
        BasicBlock *false_block;

        Branch(Value *cond, BasicBlock *true_block, BasicBlock *false_block, BasicBlock *cur_bb);

        ~Branch() override = default;

        virtual explicit operator std::string() const override;
    };

    class GetElementPtr : public LLVMInstr {
    public:
        Value *src;
        bool const_dims;
        std::vector<int> dims;
        std::vector<Value *> dims_value;

        GetElementPtr(Type *type, Value *src, std::vector<int> dims, BasicBlock *cur_bb);

        GetElementPtr(Type *type, Value *src, std::vector<Value *> dims, BasicBlock *cur_bb);

        ~GetElementPtr() override = default;

        virtual explicit operator std::string() const override;

    };

    class Zext : public LLVMInstr {
    public:
        Value *src;

        Zext(Value *src, Type *dst_type, BasicBlock *cur_bb);

        ~Zext() override = default;

        virtual explicit operator std::string() const override;
    };
}

#endif //COMPILER_LLVMINSTR_H
