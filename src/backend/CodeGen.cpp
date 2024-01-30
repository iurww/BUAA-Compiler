//
// Created by wwr on 2023/11/28.
//

#include "../../include/backend/CodeGen.h"
#include "../../include/mir/IRModule.h"
#include "../../include/mir/Type.h"
#include "../../include/mir/GlobalValue.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/Constant.h"
#include "../../include/mir/BasicBlock.h"
#include "../../include/mir/LLVMInstr.h"
#include "../../include/backend/MIPSInstr.h"
#include "../../include/midend/DFG.h"
#include <iostream>
#include <cstring>

using namespace MIPS;

CodeGen::CodeGen() {
    REG_POOL = {
            3,
            8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23,
            24, 25,
            26, 27, 28, 30};
    CONST_POOL = {
            5, 6, 7
    };
    REG_NAME = {
            "zero",
            "at",
            "v0", "v1",
            "a0", "a1", "a2", "a3",
            "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
            "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
            "t8", "t9",
            "k0", "k1",
            "gp", "sp", "fp", "ra",
    };
}

void CodeGen::gen_func(Function *func) {
    offset_sp = -8;
    for (int i = func->params.size() - 1; i >= 0; i--) {
        alloc_mem(func->params[i]);
    }
    text.push_back(get_mips_label(func->name.substr(1)));
    for (const auto &bb: func->blocks) {
        gen_bb(bb);
    }
}

void CodeGen::gen_bb(BasicBlock *bb) {
    memset(reg2val, -1, sizeof reg2val);
    text.push_back(get_mips_label(bb->name.substr(1)));
    for (const auto &instr: bb->instructions) {
//        text.push_back(get_mips_label(instr->name));
        if (auto *alu_instr = dynamic_cast<LLVM::Alu *>(instr)) {
            gen_alu(alu_instr);
        } else if (auto *icmp_instr = dynamic_cast<LLVM::Icmp *>(instr)) {
            gen_icmp(icmp_instr);
        } else if (auto *load_instr = dynamic_cast<LLVM::Load *>(instr)) {
            gen_load(load_instr);
        } else if (auto *store_instr = dynamic_cast<LLVM::Store *>(instr)) {
            gen_store(store_instr);
        } else if (auto *alloca_instr = dynamic_cast<LLVM::Alloca *>(instr)) {
            gen_alloca(alloca_instr);
        } else if (auto *jump_instr = dynamic_cast<LLVM::Jump *>(instr)) {
            gen_jump(jump_instr);
        } else if (auto *branch_instr = dynamic_cast<LLVM::Branch *>(instr)) {
            gen_branch(branch_instr);
        } else if (auto *call_instr = dynamic_cast<LLVM::Call *>(instr)) {
            gen_call(call_instr);
        } else if (auto *ret_instr = dynamic_cast<LLVM::Return *>(instr)) {
            gen_return(ret_instr);
        } else if (auto *gep_instr = dynamic_cast<LLVM::GetElementPtr *>(instr)) {
            gen_gep(gep_instr);
        } else if (auto *zext_instr = dynamic_cast<LLVM::Zext *>(instr)) {
            gen_zext(zext_instr);
        }
    }
    val2reg.clear();
}

void CodeGen::store_all() {
    for (int i = 0; i < REG_POOL.size(); i++) {
        if (reg2val[REG_POOL[i]] > 0) {
            text.push_back(get_mips_store(REG_POOL[i], 29, val2addr[reg2val[REG_POOL[i]]]));
        }
    }
}

void CodeGen::load_all() {
    for (int i = 0; i < REG_POOL.size(); i++) {
        if (reg2val[REG_POOL[i]] > 0) {
            text.push_back(get_mips_load(REG_POOL[i], 29, val2addr[reg2val[REG_POOL[i]]]));
        }
    }
}

void CodeGen::gen_alu(LLVM::Alu *alu_instr) {
    int src1 = get_reg_for(alu_instr->src[0], true);
    int src2 = get_reg_for(alu_instr->src[1], true);
    int dst = get_reg_for(alu_instr, false);
//    load_to_reg(8, alu_instr->src[0]);
//    load_to_reg(9, alu_instr->src[1]);
    int offset = alloc_mem(alu_instr);
    if (alu_instr->op == LLVM::Alu::AluOp::add) {
        text.push_back(get_mips_addu(dst, src1, src2));
    } else if (alu_instr->op == LLVM::Alu::AluOp::sub) {
        text.push_back(get_mips_subu(dst, src1, src2));
    } else if (alu_instr->op == LLVM::Alu::AluOp::mul) {
        text.push_back(get_mips_mult(src1, src2));
        text.push_back(get_mips_mflo(dst));
    } else if (alu_instr->op == LLVM::Alu::AluOp::sdiv) {
        text.push_back(get_mips_div(src1, src2));
        text.push_back(get_mips_mflo(dst));
    } else if (alu_instr->op == LLVM::Alu::AluOp::srem) {
        text.push_back(get_mips_div(src1, src2));
        text.push_back(get_mips_mfhi(dst));
    }
//    text.push_back(get_mips_store(dst, 29, offset));
}

void CodeGen::gen_icmp(LLVM::Icmp *icmp_instr) {
    int src1 = get_reg_for(icmp_instr->src[0], true);
    int src2 = get_reg_for(icmp_instr->src[1], true);
    int dst = get_reg_for(icmp_instr, false);
    int offset = alloc_mem(icmp_instr);
    if (icmp_instr->op == LLVM::Icmp::IcmpOp::slt) {
        text.push_back(get_mips_slt(dst, src1, src2));
    } else if (icmp_instr->op == LLVM::Icmp::IcmpOp::sle) {
        text.push_back(get_mips_sle(dst, src1, src2));
    } else if (icmp_instr->op == LLVM::Icmp::IcmpOp::sgt) {
        text.push_back(get_mips_sgt(dst, src1, src2));
    } else if (icmp_instr->op == LLVM::Icmp::IcmpOp::sge) {
        text.push_back(get_mips_sge(dst, src1, src2));
    } else if (icmp_instr->op == LLVM::Icmp::IcmpOp::eq) {
        text.push_back(get_mips_seq(dst, src1, src2));
    } else if (icmp_instr->op == LLVM::Icmp::IcmpOp::ne) {
        text.push_back(get_mips_sne(dst, src1, src2));
    }
//    text.push_back(get_mips_store(10, 29, offset));
}

void CodeGen::gen_alloca(LLVM::Alloca *alloca_instr) {
    int offset = alloc_mem(alloca_instr);
    int size = alloca_instr->content_type->get_full_size() * 4;
    offset_sp -= size;
    int dst = get_reg_for(alloca_instr, false);
    text.push_back(get_mips_addi(dst, 29, offset-size));
//    text.push_back(get_mips_store(dst, 29, offset));
}

void CodeGen::gen_load(LLVM::Load *load_instr) {
    int offset = alloc_mem(load_instr);
//    load_to_reg(8, load_instr->src);
    int src = get_reg_for(load_instr->src, true);
    int dst = get_reg_for(load_instr, false);
    text.push_back(get_mips_load(dst, src, 0));
//    text.push_back(get_mips_store(9, 29, offset));
}

void CodeGen::gen_store(LLVM::Store *store_instr) {
    int src = get_reg_for(store_instr->src, true);
    int dst = get_reg_for(store_instr->dst, true);
//    load_to_reg(8, store_instr->src);
//    load_to_reg(9, store_instr->dst);
    text.push_back(get_mips_store(src, dst, 0));
}

void CodeGen::gen_jump(LLVM::Jump *jump_instr) {
    store_all();
    text.push_back(get_mips_j(to_mips_label(jump_instr->dst->name)));
}

void CodeGen::gen_branch(LLVM::Branch *branch_instr) {
    int cond = get_reg_for(branch_instr->cond, true);
    store_all();
//    load_to_reg(8, branch_instr->cond);
    text.push_back(get_mips_bne(cond, 0, to_mips_label(branch_instr->true_block->name)));
    text.push_back(get_mips_j(to_mips_label(branch_instr->false_block->name)));
}

void CodeGen::gen_call(LLVM::Call *call_instr) {
    if (call_instr->func->name == "@getint") {
        gen_getint();
    } else if (call_instr->func->name == "@putint") {
        int reg = get_reg_for(call_instr->rparams[0], true);
        text.push_back(get_mips_move(4, reg));
//        load_to_reg(4, call_instr->rparams[0]);
        gen_putint();
    } else if (call_instr->func->name == "@putch") {
        int reg = get_reg_for(call_instr->rparams[0], true);
        text.push_back(get_mips_move(4, reg));
//        load_to_reg(4, call_instr->rparams[0]);
        gen_putch();
    } else {
        store_all();
        text.push_back(get_mips_store(31, 29, offset_sp - 4));
        text.push_back(get_mips_store(29, 29, offset_sp - 8));
        for (int i = call_instr->rparams.size() - 1, cnt = 0; i >= 0; i--) {
            int src = get_reg_for(call_instr->rparams[i], true);
//            load_to_reg(8, call_instr->rparams[i]);
            text.push_back(get_mips_store(src, 29, offset_sp - 4 * cnt - 12));
            cnt++;
        }
        text.push_back(get_mips_addi(29, 29, offset_sp));
        text.push_back(get_mips_jal(to_mips_label(call_instr->func->name)));
        text.push_back(get_mips_load(31, 29, -4));
        text.push_back(get_mips_load(29, 29, -8));
        load_all();
    }
    if (!call_instr->is_void) {
        int offset = alloc_mem(call_instr);
        int dst = get_reg_for(call_instr, false);
        text.push_back(get_mips_move(dst, 2));
//        text.push_back(get_mips_store(2, 29, offset));
    }
}

void CodeGen::gen_return(LLVM::Return *return_instr) {
    if (is_main) {
        text.push_back(get_mips_li(2, 10));
        text.push_back(get_mips_syscall());
    } else {
        if (return_instr->ret_value != nullptr) {
            int reg = get_reg_for(return_instr->ret_value, true);
//            load_to_reg(2, return_instr->ret_value);
            text.push_back(get_mips_move(2, reg));
        }
        text.push_back(get_mips_jr(31));
    }
}

void CodeGen::gen_gep(LLVM::GetElementPtr *gep_instr) {
        int base = get_reg_for(gep_instr->src, true);
        int dst = get_reg_for(gep_instr, false);
//    if (dynamic_cast<LLVMInstr *>(gep_instr->src)) {
//        load_to_reg(8, gep_instr->src);
//    } else {
//        text.push_back(get_mips_la(8, to_mips_var(gep_instr->src->name)));
//    }
    PointerType *type = dynamic_cast<PointerType *>(gep_instr->src->type);
    int offset = alloc_mem(gep_instr);
    if (type->ref_type->is_array_type()) {
        ArrayType *array_type = dynamic_cast<ArrayType *>(type->ref_type);
        int full_size = array_type->get_full_size();
        std::vector<int> &dims = array_type->dims;
        if (gep_instr->const_dims) {
            int pos = 0;
            for (int i = 0; i < gep_instr->dims.size(); i++) {
                pos += gep_instr->dims[i] * full_size * 4;
                if (i < dims.size()) {
                    full_size /= dims[i];
                }
            }
            text.push_back(get_mips_addi(dst, base, pos));
        } else {
            text.push_back(get_mips_move(6, base));
            for (int i = 0; i < gep_instr->dims_value.size(); i++) {
                if (auto *cv = dynamic_cast<ConstantInt *> (gep_instr->dims_value[i])) {
                    if (cv->val != 0)
                        text.push_back(get_mips_addi(6, 6, cv->val * full_size * 4));
                } else {
                    load_to_reg(7, new ConstantInt(full_size * 4));
                    int dim = get_reg_for(gep_instr->dims_value[i], true);
                    text.push_back(get_mips_mult(7, dim));
                    text.push_back(get_mips_mflo(5));
                    text.push_back(get_mips_addu(6, 6, 5));
//                    text.push_back(get_mips_li(10, full_size * 4));
//                    load_to_reg(11, gep_instr->dims_value[i]);
//                    text.push_back(get_mips_mult(10, 11));
//                    text.push_back(get_mips_mflo(12));
//                    text.push_back(get_mips_addu(8, 8, 12));
                }
                if (i < dims.size()) {
                    full_size /= dims[i];
                }
            }
            text.push_back(get_mips_move(dst, 6));
        }
    } else {
        text.push_back(get_mips_move(6, base));
        if (gep_instr->const_dims) {
            text.push_back(get_mips_addi(6, 6, gep_instr->dims[0] * 4));
        } else {
//            int temp = get_reg_for(gep_instr->dims_value[0], true);
            if (auto *cv = dynamic_cast<ConstantInt *> (gep_instr->dims_value[0])) {
                if (cv->val != 0)
                    text.push_back(get_mips_addi(6, 6, cv->val * 4));
            } else {
//                load_to_reg(7, gep_instr->dims_value[0]);
                int temp = get_reg_for(gep_instr->dims_value[0], true);
                text.push_back(get_mips_sll(temp, temp, 2));
                text.push_back(get_mips_addu(6, 6, temp));
            }
        }
        text.push_back(get_mips_move(dst, 6));
    }

//    text.push_back(get_mips_store(base, 29, offset));
}

void CodeGen::gen_zext(LLVM::Zext *zext_instr) {
    int offset = alloc_mem(zext_instr);
    int src= get_reg_for(zext_instr->src, true);
    int dst = get_reg_for(zext_instr, false);
    text.push_back(get_mips_move(dst, src));

//    load_to_reg(8, zext_instr->src);
//    text.push_back(get_mips_store(8, 29, offset));
}

void CodeGen::gen_getint() {
//    text.push_back(get_mips_label("getint"));
    text.push_back(get_mips_li(2, 5));
    text.push_back(get_mips_syscall());
//    text.push_back(get_mips_jr(31));
}

void CodeGen::gen_putint() {
//    text.push_back(get_mips_label("putint"));
    text.push_back(get_mips_li(2, 1));
//    text.push_back(get_mips_load(4, 29, -12));
    text.push_back(get_mips_syscall());
//    text.push_back(get_mips_jr(31));
}

void CodeGen::gen_putch() {
//    text.push_back(get_mips_label("putch"));
    text.push_back(get_mips_li(2, 11));
//    text.push_back(get_mips_load(4, 29, -12));
    text.push_back(get_mips_syscall());
//    text.push_back(get_mips_jr(31));
}

int CodeGen::get_reg_for(Value *val, bool need_load) {
    auto *cv = dynamic_cast<ConstantInt *>(val);
    auto *gv = dynamic_cast<GlobalValue *>(val);
    auto *iv = dynamic_cast<LLVMInstr *>(val);
    auto *pv = dynamic_cast<Function::Param *>(val);
    if (cv != nullptr) {
        int reg = CONST_POOL[const_alloc];
        const_alloc = (const_alloc + 1) % CONST_POOL.size();
        if (cv->val == 0) return 0;
        load_to_reg(reg, val);
        reg2val[reg] = -2;
        return reg;
    } else if (gv != nullptr) {
        int reg = CONST_POOL[const_alloc];
        const_alloc = (const_alloc + 1) % CONST_POOL.size();
        load_to_reg(reg, val);
        reg2val[reg] = -3;
        return reg;
    } else if (iv != nullptr || pv != nullptr) {
        if (val2reg.count(val->id)) {
            return val2reg[val->id];
        } else {
            int reg = REG_POOL[reg_alloc];
            reg_alloc = (reg_alloc + 1) % REG_POOL.size();
            int old_val = reg2val[reg];
            if (old_val > 0) {
                text.push_back(get_mips_store(reg, 29, val2addr[old_val]));
                val2reg.erase(old_val);
            }
            val2reg[val->id] = reg;
            reg2val[reg] = val->id;
            if (need_load) load_to_reg(reg, val);
            return reg;
        }
    }
    return 0;
}

void CodeGen::load_to_reg(int reg, Value *src) {
    if (auto *cv = dynamic_cast<ConstantInt *>(src)) {
        text.push_back(get_mips_li(reg, cv->val));
    } else if (auto *iv = dynamic_cast<LLVMInstr *>(src)) {
        int offset = val2addr[src->id];
        text.push_back(get_mips_load(reg, 29, offset));
    } else if (auto *gv = dynamic_cast<GlobalValue *>(src)) {
        text.push_back(get_mips_la(reg, to_mips_var(src->name)));
    } else if (auto *pv = dynamic_cast<Function::Param *>(src)) {
        int offset = val2addr[src->id];
        text.push_back(get_mips_load(reg, 29, offset));
    }
}

int CodeGen::alloc_mem(Value *value) {
    offset_sp -= 4;
    val2addr[value->id] = offset_sp;
    return offset_sp;
}

void CodeGen::gen_data() {
    for (const auto &it: IRModule::IR_MODULE->globals) {
        data.push_back(new GlobalData(it.first, it.second->init));
    }
}

void CodeGen::gen_text() {
    is_main = true;
    gen_func(IR->functions["@main"]);
    is_main = false;
    for (const auto &it: IR->functions) {
        if (it.first != "@main")
            gen_func(it.second);
    }

    gen_getint();
    gen_putint();
    gen_putch();
}

void CodeGen::gen_mips() {
    gen_data();
    gen_text();
    optimize();
//    DFG *dfg = new DFG(IR->functions);
//    dfg->run();
}

void CodeGen::optimize() {
    auto it = text.begin();
    while (it != text.end()) {
        if (MIPS::LoadStore *cur_ls = dynamic_cast<MIPS::LoadStore *>(*it)) {
            auto next = std::next(it);
            if (next != text.end()) {
                if (MIPS::LoadStore *next_ls = dynamic_cast<MIPS::LoadStore *>(*next)) {
                    if (cur_ls->reg == next_ls->reg &&
                        cur_ls->addr_reg == next_ls->addr_reg &&
                        cur_ls->offset == next_ls->offset) {
                        it = text.erase(next);
                        continue;
                    }
                }
            }
        }
        ++it;
    }
}

std::string CodeGen::to_mips_label(std::string &llvm_label) {
    return "label_" + llvm_label.substr(1);
}

std::string CodeGen::to_mips_var(std::string &llvm_var) {
    return llvm_var.substr(1) + "_var";
}

std::ostream &operator<<(std::ostream &os, const CodeGen &mips) {
    os << ".data" << std::endl;
    for (const auto &var: mips.data) {
        os << "\t" << std::string(*var) << std::endl;
    }

    os << "\n.text" << std::endl;
    for (const auto &instr: mips.text) {
        std::string s = std::string(*instr);
        if (s.substr(0, 6) != "label_") {
            os << '\t';
        }
        os << s << std::endl;
    }
    return os;
}


