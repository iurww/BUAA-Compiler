//
// Created by wwr on 2023/11/27.
//

#ifndef COMPILER_MIPSINSTR_H
#define COMPILER_MIPSINSTR_H

#include <string>
#include <vector>
#include <functional>

class Initial;

namespace MIPS {

    class MIPSInstr {
    public:
        std::string name;

        MIPSInstr(std::string name) : name(name) {}

        virtual ~MIPSInstr() = default;

        virtual explicit operator std::string() const {
            return "abstract mips instruction";
        }
    };

    class GlobalData : public MIPSInstr {
    public:
        std::vector<std::string> values;

        GlobalData(std::string name, Initial *init);

        ~GlobalData() override = default;

        virtual explicit operator std::string() const override;
    };

    class Branch : public MIPSInstr {
    public:
        int src1;
        int src2;
        std::string tag;

        Branch(std::string name, int src1, int src2, std::string tag);

        Branch(std::string name, int src, std::string tag);

        ~Branch() override = default;

        virtual explicit operator std::string() const override;
    };

    class LoadStore : public MIPSInstr {
    public:
        int reg;
        int addr_reg;
        int offset;

        LoadStore(std::string name, int reg, int addr, int offset);

        ~LoadStore() override = default;

        virtual explicit operator std::string() const override;
    };

    class CalRegReg : public MIPSInstr {
    public:
        int dst;
        int src1;
        int src2;

        CalRegReg(std::string name, int dst, int src1, int src2);

        ~CalRegReg() override = default;

        virtual explicit operator std::string() const override;
    };

    class CalRegImm : public MIPSInstr {
    public:
        int dst;
        int src;
        int imm;

        CalRegImm(std::string name, int dst, int src, int imm);

        ~CalRegImm() override = default;

        virtual explicit operator std::string() const override;
    };

    class MulDiv : public MIPSInstr {
    public:
        int src1;
        int src2;

        MulDiv(std::string name, int src1, int src2);

        ~MulDiv() override = default;

        virtual explicit operator std::string() const override;
    };

    class Move : public MIPSInstr {
    public:
        int dst;
        int src;

        Move(std::string name, int dst, int src);

        ~Move() override = default;

        virtual explicit operator std::string() const override;
    };

    class Other : public MIPSInstr {
    public:
        using display = std::function<std::string(std::string, std::string, int, int)>;
        int value1;
        int value2;
        std::string tag;
        display action;

        Other(std::string name, int value1, int value2, std::string tag, display action);

        ~Other() override = default;

        virtual explicit operator std::string() const override;
    };

    MIPSInstr *get_mips_addu(int dst, int src1, int src2);

    MIPSInstr *get_mips_subu(int dst, int src1, int src2);

    MIPSInstr *get_mips_mult(int src1, int src2);

    MIPSInstr *get_mips_div(int src1, int src2);

    MIPSInstr *get_mips_addi(int dst, int src, int imm);

    MIPSInstr *get_mips_addiu(int dst, int src, int imm);

    MIPSInstr *get_mips_sll(int dst, int src, int imm);

    MIPSInstr *get_mips_slt(int dst, int src1, int src2);

    MIPSInstr *get_mips_sle(int dst, int src1, int src2);

    MIPSInstr *get_mips_sgt(int dst, int src1, int src2);

    MIPSInstr *get_mips_sge(int dst, int src1, int src2);

    MIPSInstr *get_mips_seq(int dst, int src1, int src2);

    MIPSInstr *get_mips_sne(int dst, int src1, int src2);

    MIPSInstr *get_mips_load(int reg, int addr, int offset);

    MIPSInstr *get_mips_store(int reg, int addr, int offset);

    MIPSInstr *get_mips_move(int dst, int src);

    MIPSInstr *get_mips_mfhi(int dst);

    MIPSInstr *get_mips_mflo(int dst);

    MIPSInstr *get_mips_beq(int src1, int src2, std::string tag);

    MIPSInstr *get_mips_bne(int src1, int src2, std::string tag);

    MIPSInstr *get_mips_j(std::string tag);

    MIPSInstr *get_mips_jal(std::string tag);

    MIPSInstr *get_mips_jr(int reg);

    MIPSInstr *get_mips_syscall();

    MIPSInstr *get_mips_li(int reg, int imm);

    MIPSInstr *get_mips_la(int reg, std::string tag);

    MIPSInstr *get_mips_label(std::string name);

}

#endif //COMPILER_MIPSINSTR_H
