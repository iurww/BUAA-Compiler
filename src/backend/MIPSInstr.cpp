//
// Created by wwr on 2023/11/28.
//

#include "../../include/backend/MIPSInstr.h"
#include "../../include/mir/Initial.h"
#include "../../include/mir/Type.h"
#include "../../include/backend/CodeGen.h"
#include "../../include/util/config.h"

using namespace MIPS;

std::string reg_name(int reg) {
    if (reg < 0) return "";
#ifdef OUTPUT_MIPS_REG_NAME
    return CodeGen::REG_NAME[reg];
#else
    return std::to_string(reg);
#endif
}

GlobalData::GlobalData(std::string name, Initial *init) : MIPSInstr(name) {
    this->name = name.substr(1) + "_var";
    if (auto *iv = dynamic_cast<InitialVal *>(init)) {
        values.push_back(iv->value->name);
    } else if (auto *ia = dynamic_cast<InitialArray *>(init)) {
        for (const auto &it: ia->init_val)
            values.push_back(it->name);
    } else if (auto *iz = dynamic_cast<InitialZero *>(init)) {
        int size = iz->type->get_full_size();
        for (int i = 0; i < size; i++) {
            values.push_back("0");
        }
    }
}

GlobalData::operator std::string() const {
    std::string s = "";
    s.append(name).append(": .word");
    for (const auto v: values) s.append(" ").append(v);
    return s;
}


Branch::Branch(std::string name, int src1, int src2, std::string tag) : MIPSInstr(name), tag(tag) {
    this->src1 = src1;
    this->src2 = src2;
}

Branch::Branch(std::string name, int src, std::string tag) : MIPSInstr(name), tag(tag) {
    this->src1 = src;
}

Branch::operator std::string() const {
    std::string s1 = reg_name(src1);
    std::string s2 = reg_name(src2);
    return name + " $" + s1 + (src2 >= 0 ? (", $" + s2) : "") + ", " + tag;
}


LoadStore::LoadStore(std::string name, int reg, int addr, int offset) : MIPSInstr(name) {
    this->reg = reg;
    this->addr_reg = addr;
    this->offset = offset;
}

LoadStore::operator std::string() const {
    std::string s1 = reg_name(reg);
    std::string s2 = reg_name(addr_reg);
    return name + " $" + s1 + ", " + std::to_string(offset) + "($" + s2 + ")";
}


CalRegReg::CalRegReg(std::string name, int dst, int src1, int src2) : MIPSInstr(name) {
    this->dst = dst;
    this->src1 = src1;
    this->src2 = src2;
}

CalRegReg::operator std::string() const {
    std::string s1 = reg_name(dst);
    std::string s2 = reg_name(src1);
    std::string s3 = reg_name(src2);
    return name + " $" + s1 + ", $" + s2 + ", $" + s3;
}


CalRegImm::CalRegImm(std::string name, int dst, int src, int imm) : MIPSInstr(name) {
    this->dst = dst;
    this->src = src;
    this->imm = imm;
}

CalRegImm::operator std::string() const {
    std::string s1 = reg_name(dst);
    std::string s2 = reg_name(src);
    return name + " $" + s1 + ", $" + s2 + ", " + std::to_string(imm);
}


MulDiv::MulDiv(std::string name, int src1, int src2) : MIPSInstr(name) {
    this->src1 = src1;
    this->src2 = src2;
}

MulDiv::operator std::string() const {
    std::string s1 = reg_name(src1);
    std::string s2 = reg_name(src2);
    return name + " $" + s1 + ", $" + s2;
}


Move::Move(std::string name, int dst, int src) : MIPSInstr(name) {
    this->dst = dst;
    this->src = src;
}

Move::operator std::string() const {
    std::string s1 = reg_name(dst);
    std::string s2 = reg_name(src);
    return name + " $" + s1 + (src >= 0 ? (", $" + s2) : "");
}


Other::Other(std::string name, int value1, int value2, std::string tag, MIPS::Other::display action) : MIPSInstr(name) {
    this->value1 = value1;
    this->value2 = value2;
    this->tag = tag;
    this->action = action;
}

Other::operator std::string() const {
    return action(name, tag, value1, value2);
}

MIPSInstr *MIPS::get_mips_addu(int dst, int src1, int src2) {
    return new CalRegReg("addu", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_subu(int dst, int src1, int src2) {
    return new CalRegReg("subu", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_mult(int src1, int src2) {
    return new MulDiv("mult", src1, src2);
}

MIPSInstr *MIPS::get_mips_div(int src1, int src2) {
    return new MulDiv("div", src1, src2);
}

MIPSInstr *MIPS::get_mips_sll(int dst, int src, int imm) {
    return new CalRegImm("sll", dst, src, imm);
}

MIPSInstr *MIPS::get_mips_slt(int dst, int src1, int src2) {
    return new CalRegReg("slt", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_sle(int dst, int src1, int src2) {
    return new CalRegReg("sle", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_sgt(int dst, int src1, int src2) {
    return new CalRegReg("sgt", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_sge(int dst, int src1, int src2) {
    return new CalRegReg("sge", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_seq(int dst, int src1, int src2) {
    return new CalRegReg("seq", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_sne(int dst, int src1, int src2) {
    return new CalRegReg("sne", dst, src1, src2);
}

MIPSInstr *MIPS::get_mips_load(int reg, int addr, int offset) {
    return new LoadStore("lw", reg, addr, offset);
}

MIPSInstr *MIPS::get_mips_store(int reg, int addr, int offset) {
    return new LoadStore("sw", reg, addr, offset);
}

MIPSInstr *MIPS::get_mips_move(int dst, int src) {
    return new Move("move", dst, src);
}

MIPSInstr *MIPS::get_mips_mfhi(int dst) {
    return new Move("mfhi", dst, -1);
}

MIPSInstr *MIPS::get_mips_mflo(int dst) {
    return new Move("mflo", dst, -1);
}

MIPSInstr *MIPS::get_mips_beq(int src1, int src2, std::string tag) {
    return new Branch("beq", src1, src2, tag);
}

MIPSInstr *MIPS::get_mips_bne(int src1, int src2, std::string tag) {
    return new Branch("bne", src1, src2, tag);
}

MIPSInstr *MIPS::get_mips_j(std::string tag) {
    return new Other("j", -1, -1, tag,
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         return name + " " + tag;
                     });
}

MIPSInstr *MIPS::get_mips_jal(std::string tag) {
    return new Other("jal", -1, -1, tag,
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         return name + " " + tag;
                     });
}

MIPSInstr *MIPS::get_mips_jr(int reg) {
    return new Other("jr", reg, -1, "",
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         std::string s1 = value1 >= 0 ? reg_name(value1) : "";
                         return name + " $" + s1;
                     });
}

MIPSInstr *MIPS::get_mips_syscall() {
    return new Other("syscall", -1, -1, "",
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         return name;
                     });
}

MIPSInstr *MIPS::get_mips_addi(int dst, int src, int imm) {
    return new CalRegImm("addi", dst, src, imm);
}

MIPSInstr *MIPS::get_mips_label(std::string name) {
    return new Other(name, -1, -1, "",
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         return "label_" + name + ":";
                     });
}

MIPSInstr *MIPS::get_mips_li(int reg, int imm) {
    return new Other("li", reg, imm, "",
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         std::string s1 = value1 >= 0 ? reg_name(value1) : "";
                         return name + " $" + s1 + ", " + std::to_string(value2);
                     });
}

MIPSInstr *MIPS::get_mips_la(int reg, std::string tag) {
    return new Other("la", reg, -1, tag,
                     [](const std::string &name, const std::string &tag, int value1, int value2) {
                         std::string s1 = value1 >= 0 ? reg_name(value1) : "";
                         return name + " $" + s1 + ", " + tag;
                     });
}
