//
// Created by wwr on 2023/10/9.
//

#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include "Token.h"

namespace AST {

    class ASTNode;

    class CompUnit;

    class Decl;

    class FuncDef;

    class Def;

    class FuncFParam;

    class Block;

    class BlockItem;

    class Stmt;

    class Exp;

    class Init;

    class InitArray;

    class LVal;

    class AssignStmt;

    class ExpStmt;

    class IfStmt;

    class ForStmt;

    class ReturnStmt;

    class BreakStmt;

    class ContinueStmt;

    class PrimaryExp;

    class PrintfStmt;

    class Number;

    class Call;

    class UnaryExp;

    class BinaryExp;

    class ASTNode {
    public:
    };

    class CompUnit {
    public:
        std::vector<Decl *> decls;
        std::vector<FuncDef *> func_defs;

        CompUnit() = default;

        ~CompUnit() = default;
    };

    class FuncDef {
    public:
        Token *type;
        Token *ident;
        std::vector<FuncFParam *> fparams;
        Block *body;

        FuncDef() : type(nullptr), ident(nullptr), body(nullptr) {
        }

        ~FuncDef() = default;
    };

    class Def {
    public:
        Token *type;
        Token *ident;
        std::vector<Exp *> sizes;
        Init *init;

        Def() : type(nullptr), ident(nullptr), init(nullptr) {

        }

        ~Def() = default;

    };

    class FuncFParam {
    public:
        Token *type;
        Token *ident;
        bool is_array;
        std::vector<Exp *> sizes;

        FuncFParam() : type(nullptr), ident(nullptr) {
            this->is_array = false;
        }

        ~FuncFParam() = default;

    };

    class BlockItem {
    public:
        BlockItem() = default;

        virtual ~BlockItem() = default;
    };

    class Decl : public BlockItem {
    public:
        bool is_const;
        Token *type;
        std::vector<Def *> defs;

        Decl() : type(nullptr) {
            this->is_const = false;
        }

        ~Decl() override = default;
    };

    class Init {
    public:
        Init() = default;

        virtual ~Init() = default;
    };

    class PrimaryExp {
    public:
        PrimaryExp() = default;

        virtual ~PrimaryExp() = default;
    };

    class Exp : public Init, public PrimaryExp {
    public:
        Exp() = default;

        ~Exp() override = default;
    };

    class InitArray : public Init {
    public:
        std::vector<Init *> inits;

        InitArray() = default;

        ~InitArray() override = default;
    };

    class Stmt : public BlockItem {
    public:
        Stmt() = default;

        ~Stmt() override = default;
    };

    class LVal : public PrimaryExp {
    public:
        Token *ident;
        std::vector<Exp *> indexes;

        LVal() = default;

        ~LVal() override = default;
    };

    class AssignStmt : public Stmt {
    public:
        LVal *lval;
        Exp *exp;
        bool is_input;

        AssignStmt() : lval(nullptr), exp(nullptr) {
            this->is_input = false;
        }

        ~AssignStmt() override = default;
    };

    class Block : public Stmt {
    public:
        std::vector<BlockItem *> items;
        Token *end_rbrace;

        Block() = default;

        ~Block() override = default;
    };

    class ExpStmt : public Stmt {
    public:
        Exp *exp;

        ExpStmt() : exp(nullptr) {}

        ~ExpStmt() override = default;
    };

    class IfStmt : public Stmt {
    public:
        Exp *cond;
        Stmt *then_stmt;
        Stmt *else_stmt;

        IfStmt() : cond(nullptr), then_stmt(nullptr), else_stmt(nullptr) {}

        ~IfStmt() override = default;
    };

    class ForStmt : public Stmt {
    public:
        AssignStmt *exp1;
        Exp *exp2;
        AssignStmt *exp3;
        Stmt *body;

        ForStmt() : exp1(nullptr), exp2(nullptr), exp3(nullptr), body(nullptr) {}

        ~ForStmt() override = default;
    };

    class ReturnStmt : public Stmt {
    public:
        Token *ret_token;
        Exp *exp;

        ReturnStmt() : exp(nullptr) {}

        ~ReturnStmt() override = default;
    };

    class BreakStmt : public Stmt {
    public:
        Token *break_token;

        BreakStmt() = default;

        ~BreakStmt() override = default;
    };

    class ContinueStmt : public Stmt {
    public:
        Token *continue_token;

        ContinueStmt() = default;

        ~ContinueStmt() override = default;
    };

    class PrintfStmt : public Stmt {
    public:
        Token *fs;
        std::vector<Exp *> params;

        PrintfStmt() : fs(nullptr) {}

        ~PrintfStmt() override = default;
    };

    class Number : public PrimaryExp {
    public:
        Token *token;
        int val;

        Number() : token(nullptr), val(0) {}

        ~Number() override = default;
    };

    class Call : public PrimaryExp {
    public:
        Token *ident;
        std::vector<Exp *> rparams;

        Call() : ident(nullptr) {}

        ~Call() override = default;
    };

    class UnaryExp : public Exp {
    public:
        std::vector<Token *> unary_ops;
        PrimaryExp *primary_exp;

        UnaryExp() : primary_exp(nullptr) {}

        ~UnaryExp() override = default;
    };

    class BinaryExp : public Exp {
    public:
        Exp *first;
        std::vector<Token *> binary_ops;
        std::vector<Exp *> follows;

        BinaryExp() : first(nullptr) {}

        ~BinaryExp() override = default;
    };

}

#endif //COMPILER_AST_H
