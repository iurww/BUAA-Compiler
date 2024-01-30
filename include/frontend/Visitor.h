//
// Created by wwr on 2023/11/6.
//

#ifndef COMPILER_VISITOR_H
#define COMPILER_VISITOR_H

#include <stack>
#include "AST.h"
#include "../mir/IRModule.h"
#include "../mir/LLVMInstr.h"

class Value;

class Type;

class BasicType;

class ArrayType;

class InitialExp;

class InitialVal;

class InitialArray;

class BasicBlock;

class Function;

class SymbolTable;

class IRModule;

class Visitor {
public:
    static SymbolTable *cur_sym_table;
    bool is_global;
    int in_loop;

    BasicBlock *cur_bb;
    Function *cur_func;

    std::stack<BasicBlock *> continue_blocks;
    std::stack<BasicBlock *> break_blocks;

    IRModule *IR = IRModule::IR_MODULE;

    Visitor();

    ~Visitor() = default;

    void visitCompUnit(AST::CompUnit *root);

    void visitDecl(AST::Decl *decl);

    void visitFuncDef(AST::FuncDef *func_def);

    void visitDef(AST::Def *def, bool is_const);

    void visitBlock(AST::Block *block, bool need_new_sym_table);

    void visitBlockItem(AST::BlockItem *item);

    void visitStmt(AST::Stmt *stmt);

    void visitAssignStmt(AST::AssignStmt *assign);

    void visitExpStmt(AST::ExpStmt *exp_stmt);

    void visitIfStmt(AST::IfStmt *if_stmt);

    void visitForStmt(AST::ForStmt *for_stmt);

    void visitReturnStmt(AST::ReturnStmt *return_stmt);

    void visitBreakStmt(AST::BreakStmt *break_stmt);

    void visitContinueStmt(AST::ContinueStmt *continue_stmt);

    void visitPrintfStmt(AST::PrintfStmt *printf_stmt);

    InitialVal *visitInitVal(BasicType *type, AST::Exp *);

    InitialExp *visitInitExp(BasicType *type, AST::Exp *);

    InitialArray *visitInitArray(ArrayType *type, AST::InitArray *inits);

    Type *visitFuncFParam(AST::FuncFParam *fparam);

    void visitCondOr(AST::Exp *cond_or, BasicBlock *true_block, BasicBlock *false_block);

    void visitCondAnd(AST::Exp *cond_and, BasicBlock *true_block, BasicBlock *false_block);

    Value *visitExp(AST::Exp *exp);

    Value *visitBinaryExp(AST::BinaryExp *exp);

    Value *visitUnaryExp(AST::UnaryExp *exp);

    Value *visitPrimaryExp(AST::PrimaryExp *exp);

    Value *visitLVal(AST::LVal *lval, bool need_load);

    Value *visitNumber(AST::Number *num);

    Value *visitCall(AST::Call *call);

    LLVM::Alu::AluOp token_to_alu_op(Token *op);

    LLVM::Icmp::IcmpOp token_to_icmp_op(Token *op);

    void traverse(std::vector<std::vector<int>> &ans, std::vector<int> &cur, std::vector<int> &dims, int depth);

    Type *type_decay(Value *value, int x);

    Value *trim_to(Value *value);
};

#endif //COMPILER_VISITOR_H
