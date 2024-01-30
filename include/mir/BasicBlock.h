//
// Created by wwr on 2023/11/7.
//

#ifndef COMPILER_BASICBLOCK_H
#define COMPILER_BASICBLOCK_H

#include <vector>
#include <unordered_set>
#include <set>
#include "Value.h"

class LLVMInstr;

class Function;

class BasicBlock : public Value {
public:
    std::vector<LLVMInstr *> instructions;
    Function *function;
    bool is_closed;

    std::vector<BasicBlock *> next;
    std::vector<BasicBlock *> prev;

    std::unordered_set<Value *> live_def;
    std::unordered_set<Value *> live_use;

    std::set<BasicBlock *> doms;
    std::set<BasicBlock *> dom_by;
    std::set<BasicBlock *> sdoms;
    std::set<BasicBlock *> sdom_by;
    std::set<BasicBlock *> idoms;
    BasicBlock *idom_by;
    std::set<BasicBlock *> df;


    BasicBlock(Function *cur_func);

    ~BasicBlock() = default;

    void add_instr_back(LLVMInstr *instr);

    virtual explicit operator std::string() const override;

    friend std::ostream &operator<<(std::ostream &os, const BasicBlock &bb);
};


#endif //COMPILER_BASICBLOCK_H
