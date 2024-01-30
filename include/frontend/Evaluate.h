//
// Created by wwr on 2023/11/6.
//

#ifndef COMPILER_EVALUATE_H
#define COMPILER_EVALUATE_H

#include "AST.h"

namespace Evaluate {

    int eval_number(AST::Number *num);

    int eval_lval(AST::LVal *lval);

    int eval_const_exp(AST::Exp *exp);

    int eval_binary_const_exp(AST::BinaryExp *bp);

    int eval_unary_const_exp(AST::UnaryExp *up);

    int eval_primary_exp(AST::PrimaryExp *pp);

    int binary_cal(Token *op, int src1, int src2);

    int unary_cal(Token *op, int src);

}


#endif //COMPILER_EVALUATE_H
