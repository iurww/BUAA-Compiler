//
// Created by wwr on 2023/11/27.
//

#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include "../../include/mir/IRModule.h"

#include <unordered_map>

class Function;

class BasicBlock;

class Value;

namespace LLVM {
    class Alu;

    class Icmp;

    class Alloca;

    class Load;

    class Store;

    class Call;

    class Return;

    class Jump;

    class Branch;

    class GetElementPtr;

    class Zext;
}

namespace MIPS {
    class GlobalData;

    class MIPSInstr;
}

class CodeGen {
public:
    static CodeGen *CODE_GEN;
    IRModule *IR = IRModule::IR_MODULE;

    std::vector<MIPS::GlobalData *> data;
    std::vector<MIPS::MIPSInstr *> text;

    std::vector<int> REG_POOL;
    std::vector<int> CONST_POOL;
    std::vector<std::string> REG_NAME;
    std::unordered_map<int, int> val2addr;
    std::unordered_map<int, int> val2reg;
    int reg2val[32];

    int offset_sp = 0;
    bool is_main = false;
    int reg_alloc = 0;
    int const_alloc = 0;


    CodeGen();

    ~CodeGen() = default;

    void gen_data();

    void gen_text();

    void gen_mips();

    void gen_func(Function *func);

    void gen_bb(BasicBlock *bb);

    void gen_alu(LLVM::Alu *alu_instr);

    void gen_icmp(LLVM::Icmp *icmp_instr);

    void gen_alloca(LLVM::Alloca *alloca_instr);

    void gen_load(LLVM::Load *load_instr);

    void gen_store(LLVM::Store *store_instr);

    void gen_jump(LLVM::Jump *jump_instr);

    void gen_branch(LLVM::Branch *branch_instr);

    void gen_call(LLVM::Call *call_instr);

    void gen_return(LLVM::Return *ret_instr);

    void gen_gep(LLVM::GetElementPtr *gep_instr);

    void gen_zext(LLVM::Zext *zext_instr);

    void gen_getint();

    void gen_putint();

    void gen_putch();

    int get_reg_for(Value *val, bool need_load);

    void load_to_reg(int reg, Value *src);

    int alloc_mem(Value *value);

    void store_all();

    void load_all();

    std::string to_mips_label(std::string &llvm_label);

    std::string to_mips_var(std::string &llvm_var);

    void optimize();

    friend std::ostream &operator<<(std::ostream &os, const CodeGen &mips);

};

#endif //COMPILER_CODEGEN_H
