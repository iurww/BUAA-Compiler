//
// Created by wwr on 2023/11/7.
//

#include <sstream>
#include <string>
#include <iostream>
#include "../../include/mir/BasicBlock.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/Type.h"
#include "../../include/mir/LLVMInstr.h"


BasicBlock::BasicBlock(Function *cur_func) : Value(new BBType()), function(cur_func) {
    function = cur_func;
    function->add_block_back(this);
    id = VALUE_CNT;
    name = BB_NAME_PREFIX + std::to_string(VALUE_CNT++);
    is_closed = false;
    idom_by = nullptr;
}

void BasicBlock::add_instr_back(LLVMInstr *instr) {
    instructions.push_back(instr);
}

BasicBlock::operator std::string() const {
    std::ostringstream oss;
    oss << *this;
    return oss.str();
}

std::ostream &operator<<(std::ostream &os, const BasicBlock &bb) {
//    if (bb.instructions.empty()) return os;
    os << bb.name << ":" << std::endl;
    for (const auto it: bb.instructions) {
        os << "\t" << std::string(*it) << std::endl;
    }
    return os;
}