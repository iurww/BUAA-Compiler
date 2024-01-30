//
// Created by wwr on 2023/11/7.
//

#include "../../include/frontend/Evaluate.h"
#include "../../include/frontend/Symbol.h"
#include "../../include/frontend/Visitor.h"
#include "../../include/mir/Constant.h"
#include "../../include/mir/Initial.h"

int Evaluate::eval_number(AST::Number *num) {
    std::string content = num->token->content;
    return std::stoi(content);
}

int Evaluate::eval_lval(AST::LVal *lval) {
    std::string name = lval->ident->content;
    Symbol *symbol = Visitor::cur_sym_table->get(name);

    Initial *init = symbol->init;
    if (auto *iv = dynamic_cast<InitialVal *>(init)) {
        return dynamic_cast<ConstantInt *>(iv->value)->val;
    } else {
        return 2147483647;
    }
}

int Evaluate::eval_const_exp(AST::Exp *exp) {
    if (auto *bp = dynamic_cast<AST::BinaryExp *>(exp)) {
        return Evaluate::eval_binary_const_exp(bp);
    } else if (auto *up = dynamic_cast<AST::UnaryExp *>(exp)) {
        return Evaluate::eval_unary_const_exp(up);
    } else {
        exit(102);
    }
}

int Evaluate::eval_binary_const_exp(AST::BinaryExp *bp) {
    int v = Evaluate::eval_const_exp(bp->first);
    for (int i = 0; i < bp->follows.size(); i++) {
        Token *op = bp->binary_ops[i];
        AST::Exp *exp = bp->follows[i];
        v = Evaluate::binary_cal(op, v, eval_const_exp(exp));
    }
    return v;
}

int Evaluate::eval_unary_const_exp(AST::UnaryExp *up) {
    AST::PrimaryExp *pp = up->primary_exp;
    int v = Evaluate::eval_primary_exp(pp);
    for (int i = 0; i < up->unary_ops.size(); i++) {
        Token *op = up->unary_ops[i];
        v = Evaluate::unary_cal(op, v);
    }
    return v;
}


int Evaluate::binary_cal(Token *op, int src1, int src2) {
    if (op->is_of(TokenType::PLUS)) {
        return src1 + src2;
    } else if (op->is_of(TokenType::MINU)) {
        return src1 - src2;
    } else if (op->is_of(TokenType::MULT)) {
        return src1 * src2;
    } else if (op->is_of(TokenType::DIV)) {
        return src1 / src2;
    } else if (op->is_of(TokenType::MOD)) {
        return src1 % src2;
    } else {
        exit(103);
    }
}

int Evaluate::unary_cal(Token *op, int src) {
    if (op->is_of(TokenType::PLUS)) {
        return src;
    } else if (op->is_of(TokenType::MINU)) {
        return -src;
    } else if (op->is_of(TokenType::NOT)) {
        return src == 0 ? 1 : 0;
    } else {
        exit(104);
    }
}

int Evaluate::eval_primary_exp(AST::PrimaryExp *pp) {
    if (auto *num = dynamic_cast<AST::Number *>(pp)) {
        return Evaluate::eval_number(num);
    } else if (auto *lval = dynamic_cast<AST::LVal *>(pp)) {
        return Evaluate::eval_lval(lval);
    } else if (auto *exp = dynamic_cast<AST::Exp *>(pp)) {
        return Evaluate::eval_const_exp(exp);
    } else {
        exit(105);
    }
}