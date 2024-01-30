//
// Created by wwr on 2023/11/6.
//

#include "../../include/frontend/Visitor.h"
#include "../../include/mir/GlobalValue.h"
#include "../../include/frontend/Evaluate.h"
#include "../../include/mir/Type.h"
#include "../../include/frontend/Symbol.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/Constant.h"
#include "../../include/mir/BasicBlock.h"
#include "../../include/mir/Initial.h"
#include "../../include/util/config.h"
#include "../../include/frontend/Error.h"


SymbolTable *Visitor::cur_sym_table = new SymbolTable();

Visitor::Visitor() {
    is_global = true;
    in_loop = 0;
}

void Visitor::visitCompUnit(AST::CompUnit *root) {
    for (AST::Decl *decl: root->decls) {
        visitDecl(decl);
    }
    for (AST::FuncDef *func_def: root->func_defs) {
        visitFuncDef(func_def);
    }
}

void Visitor::visitDecl(AST::Decl *decl) {
    for (AST::Def *def: decl->defs) {
        visitDef(def, decl->is_const);
    }
}

void Visitor::visitFuncDef(AST::FuncDef *func_def) {
    std::string func_name = func_def->ident->content;
#ifdef ERROR_CHECK
    bool flag = ErrorHandler::ERROR_HANDLER->handle_b(cur_sym_table, func_def->ident);
    if (flag) return;
#endif

    Type *ret_type = new BasicType(func_def->type);
    FuncType *func_type = new FuncType(ret_type);
    cur_func = new Function(func_name, func_type, ret_type);

    Symbol *func_sym = new Symbol(func_name, func_type, cur_func, false, is_global, true, nullptr);
    cur_sym_table->add(func_sym);
    IR->add_function(cur_func);

    is_global = false;
    cur_sym_table = new SymbolTable(cur_sym_table);
    cur_bb = new BasicBlock(cur_func);

    for (auto *p: func_def->fparams) {
        std::string param_name = p->ident->content;
#ifdef ERROR_CHECK
        ErrorHandler::ERROR_HANDLER->handle_b(cur_sym_table, p->ident);
#endif
        Type *param_type = visitFuncFParam(p);
        func_type->add_param_type(param_type);

        Function::Param *param_value = new Function::Param(param_type);
        Value *value = new LLVM::Alloca(param_type, cur_bb);
        new LLVM::Store(param_value, value, cur_bb);

        cur_func->add_param(param_value);
        bool is_complete = !param_type->is_pointer_type(); // 数组退化为指针，数组类型形参为不完全类型
        Symbol *param_sym = new Symbol(param_name, param_type, value, false, is_global, is_complete, nullptr);
        cur_sym_table->add(param_sym);
    }

    visitBlock(func_def->body, false);
    if (!cur_bb->is_closed) {
#ifdef ERROR_CHECK
        if (!ret_type->is_void_type())
            ErrorHandler::ERROR_HANDLER->handle_g(func_def->body->end_rbrace->line);
#endif
        if (ret_type->is_void_type())
            new LLVM::Return(cur_bb);
        else if (ret_type->is_basic_type())
            new LLVM::Return(new ConstantInt(0), cur_bb);
    }

    is_global = true;
    cur_func = nullptr;
    cur_bb = nullptr;
    cur_sym_table = cur_sym_table->parent;

}

Type *Visitor::visitFuncFParam(AST::FuncFParam *fparam) {
    if (fparam->is_array) {
        Type *type = new BasicType(fparam->type);
        for (int i = fparam->sizes.size() - 1; i >= 0; i--) {
            type = new ArrayType(Evaluate::eval_const_exp(fparam->sizes[i]), type);
        }
        return new PointerType(type);

    } else {
        return new BasicType(fparam->type);
    }
}

void Visitor::visitBlock(AST::Block *block, bool need_new_sym_table) {
    if (need_new_sym_table) cur_sym_table = new SymbolTable(cur_sym_table);
    for (AST::BlockItem *item: block->items) {
        visitBlockItem(item);
    }
    if (need_new_sym_table) cur_sym_table = cur_sym_table->parent;
}

void Visitor::visitBlockItem(AST::BlockItem *item) {
    if (auto stmt = dynamic_cast<AST::Stmt *>(item)) {
        visitStmt(stmt);
    } else if (auto decl = dynamic_cast<AST::Decl *>(item)) {
        visitDecl(decl);
    } else {
        exit(93);
    }
}

void Visitor::visitStmt(AST::Stmt *stmt) {
    if (auto assign = dynamic_cast<AST::AssignStmt *>(stmt)) {
        visitAssignStmt(assign);
    } else if (auto block = dynamic_cast<AST::Block *>(stmt)) {
        visitBlock(block, true);
    } else if (auto exp_stmt = dynamic_cast<AST::ExpStmt *>(stmt)) {
        visitExpStmt(exp_stmt);
    } else if (auto if_stmt = dynamic_cast<AST::IfStmt *>(stmt)) {
        visitIfStmt(if_stmt);
    } else if (auto for_stmt = dynamic_cast<AST::ForStmt *>(stmt)) {
        visitForStmt(for_stmt);
    } else if (auto return_stmt = dynamic_cast<AST::ReturnStmt *>(stmt)) {
        visitReturnStmt(return_stmt);
    } else if (auto break_stmt = dynamic_cast<AST::BreakStmt *>(stmt)) {
        visitBreakStmt(break_stmt);
    } else if (auto continue_stmt = dynamic_cast<AST::ContinueStmt *>(stmt)) {
        visitContinueStmt(continue_stmt);
    } else if (auto printf_stmt = dynamic_cast<AST::PrintfStmt *>(stmt)) {
        visitPrintfStmt(printf_stmt);
    } else {
        exit(94);
    }
}

void Visitor::visitAssignStmt(AST::AssignStmt *assign) {
    Value *lval = visitLVal(assign->lval, false);
#ifdef ERROR_CHECK
    ErrorHandler::ERROR_HANDLER->handle_h(cur_sym_table, assign->lval->ident);
#endif
    if (assign->is_input) {
        Function *getint = IR->external_functions[0];
        Value *value = new LLVM::Call(getint, getint->ret_type, cur_bb);
        new LLVM::Store(value, lval, cur_bb);
    } else {
        Value *exp = visitExp(assign->exp);
        new LLVM::Store(exp, lval, cur_bb);
    }
}

void Visitor::visitExpStmt(AST::ExpStmt *exp_stmt) {
    AST::Exp *exp = exp_stmt->exp;
    if (exp != nullptr) {
        visitExp(exp);
    }
}

void Visitor::visitIfStmt(AST::IfStmt *if_stmt) {
    AST::Stmt *then_stmt = if_stmt->then_stmt;
    AST::Stmt *else_stmt = if_stmt->else_stmt;
    AST::Exp *cond = if_stmt->cond;

    BasicBlock *then_block = new BasicBlock(cur_func);
    BasicBlock *else_block = (else_stmt) ? new BasicBlock(cur_func) : nullptr;
    BasicBlock *after_block = new BasicBlock(cur_func);

    if (else_stmt) {
        visitCondOr(cond, then_block, else_block);

        cur_bb = then_block;
        visitStmt(then_stmt);
        new LLVM::Jump(after_block, cur_bb);

        cur_bb = else_block;
        visitStmt(else_stmt);
    } else {
        visitCondOr(cond, then_block, after_block);

        cur_bb = then_block;
        visitStmt(then_stmt);
    }

    if (!cur_bb->is_closed)
        new LLVM::Jump(after_block, cur_bb);
    cur_bb = after_block;
}

void Visitor::visitCondOr(AST::Exp *cond_or, BasicBlock *true_block, BasicBlock *false_block) {
    AST::BinaryExp *binary_exp = dynamic_cast<AST::BinaryExp *>(cond_or);
    if (binary_exp == nullptr) exit(96);

    if (binary_exp->follows.empty()) {
        visitCondAnd(binary_exp->first, true_block, false_block);
    } else {
        BasicBlock *next_first_block = new BasicBlock(cur_func);
        visitCondAnd(binary_exp->first, true_block, next_first_block);
        for (int i = 0; i < binary_exp->follows.size(); i++) {
            cur_bb = next_first_block;
            if (i < binary_exp->follows.size() - 1) {
                next_first_block = new BasicBlock(cur_func);
                visitCondAnd(binary_exp->follows[i], true_block, next_first_block);
            } else {
                visitCondAnd(binary_exp->follows[i], true_block, false_block);
            }
        }
    }
}

void Visitor::visitCondAnd(AST::Exp *cond_and, BasicBlock *true_block, BasicBlock *false_block) {
    AST::BinaryExp *binary_exp = dynamic_cast<AST::BinaryExp *>(cond_and);
    if (binary_exp == nullptr) exit(97);

    Value *first = trim_to(visitExp(binary_exp->first));
    if (binary_exp->follows.empty()) {
        new LLVM::Branch(first, true_block, false_block, cur_bb);
    } else {
        BasicBlock *next_block = new BasicBlock(cur_func);
        new LLVM::Branch(first, next_block, false_block, cur_bb);
        cur_bb = next_block;

        for (int i = 0; i < binary_exp->follows.size(); i++) {
            first = trim_to(visitExp(binary_exp->follows[i]));
            if (i < binary_exp->follows.size() - 1) {
                next_block = new BasicBlock(cur_func);
                new LLVM::Branch(first, next_block, false_block, cur_bb);
                cur_bb = next_block;
            } else {
                new LLVM::Branch(first, true_block, false_block, cur_bb);
            }
        }
    }
}

void Visitor::visitForStmt(AST::ForStmt *for_stmt) {
    if (for_stmt->exp1 != nullptr) {
        visitAssignStmt(for_stmt->exp1);
    }

    BasicBlock *cond_block = for_stmt->exp2 ? new BasicBlock(cur_func) : nullptr;
    BasicBlock *body_block = new BasicBlock(cur_func);
    BasicBlock *exp3_block = for_stmt->exp3 ? new BasicBlock(cur_func) : nullptr;
    BasicBlock *follow_block = new BasicBlock(cur_func);

    cond_block ? new LLVM::Jump(cond_block, cur_bb) : new LLVM::Jump(body_block, cur_bb);
    if (cond_block) {
        new LLVM::Jump(cond_block, cur_bb);
        cur_bb = cond_block;
        visitCondOr(for_stmt->exp2, body_block, follow_block);
    }

    cur_bb = body_block;
    BasicBlock *continue_block = exp3_block ? exp3_block : (cond_block ? cond_block : body_block);
    continue_blocks.push(continue_block);
    break_blocks.push(follow_block);
    in_loop++;
    visitStmt(for_stmt->body);
    in_loop--;
    continue_blocks.pop();
    break_blocks.pop();

    if (exp3_block != nullptr) {
        new LLVM::Jump(exp3_block, cur_bb);
        cur_bb = exp3_block;
        visitAssignStmt(for_stmt->exp3);
    }

    cond_block ? new LLVM::Jump(cond_block, cur_bb) : new LLVM::Jump(body_block, cur_bb);

    cur_bb = follow_block;
}

void Visitor::visitReturnStmt(AST::ReturnStmt *return_stmt) {
    AST::Exp *exp = return_stmt->exp;
#ifdef ERROR_CHECK
    ErrorHandler::ERROR_HANDLER->handle_f(cur_func, return_stmt->ret_token, exp);
#endif
    if (exp != nullptr) {
        Value *ret_value = visitExp(exp);
        new LLVM::Return(ret_value, cur_bb);
    } else {
        new LLVM::Return(cur_bb);
    }
}

void Visitor::visitBreakStmt(AST::BreakStmt *break_stmt) {
#ifdef ERROR_CHECK
    if (!in_loop)
        ErrorHandler::ERROR_HANDLER->handle_m(break_stmt->break_token);
#endif
    if (!break_blocks.empty())
        new LLVM::Jump(break_blocks.top(), cur_bb);
}

void Visitor::visitContinueStmt(AST::ContinueStmt *continue_stmt) {
#ifdef ERROR_CHECK
    if (!in_loop)
        ErrorHandler::ERROR_HANDLER->handle_m(continue_stmt->continue_token);
#endif
    if (!continue_blocks.empty())
        new LLVM::Jump(continue_blocks.top(), cur_bb);
}

void Visitor::visitPrintfStmt(AST::PrintfStmt *printf_stmt) {
    Function *putint = IR->external_functions[1];
    Function *putch = IR->external_functions[2];
    std::string fs = printf_stmt->fs->content;

#ifdef ERROR_CHECK
    ErrorHandler::ERROR_HANDLER->handle_l(printf_stmt->fs, printf_stmt->params.size());
#endif

    int param_cnt = 0;
    std::vector<Value *> rparams;
    for (int i = 1; i < fs.size() - 1; i++) {
        rparams.clear();
        if (fs[i] == '%' && fs[i + 1] == 'd' && param_cnt < printf_stmt->params.size()) {
            Value *param = visitExp(printf_stmt->params[param_cnt]);
            rparams.push_back(param);
            LLVM::Call *call = new LLVM::Call(putint, putint->ret_type, rparams, cur_bb);
            i++, param_cnt++;
        } else if (fs[i] == '\\') {
            rparams.push_back(new ConstantInt(int('\n')));
            LLVM::Call *call = new LLVM::Call(putch, putint->ret_type, rparams, cur_bb);
            i++;
        } else {
            rparams.push_back(new ConstantInt(int(fs[i])));
            LLVM::Call *call = new LLVM::Call(putch, putint->ret_type, rparams, cur_bb);
        }
    }
}

void Visitor::visitDef(AST::Def *def, bool is_const) {
    bool need_eval = is_const || is_global;

    std::string name = def->ident->content;
#ifdef ERROR_CHECK
    if (ErrorHandler::ERROR_HANDLER->handle_b(cur_sym_table, def->ident))
        return;
#endif
    Type *type = new BasicType(def->type);
    for (int i = def->sizes.size() - 1; i >= 0; i--) {
        int len = Evaluate::eval_const_exp(def->sizes[i]);
        type = new ArrayType(len, type);
    }
    PointerType *pointer_type = new PointerType(type);

    AST::Init *def_init = def->init;
    Initial *init = nullptr;
    if (def_init != nullptr) {
        if (type->is_basic_type()) {
            if (need_eval)
                init = visitInitVal((BasicType *) type, dynamic_cast<AST::Exp *>(def_init));
            else
                init = visitInitExp((BasicType *) type, dynamic_cast<AST::Exp *>(def_init));
        } else {
            init = visitInitArray(dynamic_cast<ArrayType *> (type), dynamic_cast<AST::InitArray *>(def_init));
        }
    }

    if (init == nullptr) {
        if (type->is_basic_type()) {
            init = new InitialVal(type, new ConstantInt(0));
        } else {
            init = new InitialZero(type);
        }
    }
    Value *value = nullptr;
    if (is_global) {
        value = new GlobalValue(name, pointer_type, init, is_const);
        IR->add_global((GlobalValue *) value);
    } else {
        if (type->is_basic_type()) {
            if (!is_const) {
                value = new LLVM::Alloca(type, cur_bb);
                new LLVM::Store(((InitialExp *) init)->result, value, cur_bb);
            }
        } else {
            value = new LLVM::Alloca(type, cur_bb);
            if (dynamic_cast<InitialArray *>(init) != nullptr) {
                std::vector<std::vector<int>> ans;
                std::vector<int> cur = {0};
                std::vector<int> dims = ((ArrayType *) type)->dims;
                traverse(ans, cur, dims, 0);
                for (int i = 0; i < ans.size(); i++) {
                    Value *element = new LLVM::GetElementPtr(new PointerType(new BasicType(TypeTag::int32_type)),
                                                             value,
                                                             ans[i], cur_bb);
                    new LLVM::Store(((InitialArray *) init)->init_val[i], element, cur_bb);
                }
            }
        }

    }

    Symbol *symbol = new Symbol(name, pointer_type, value, is_const, is_global, true, init);
    cur_sym_table->add(symbol);

}

InitialVal *Visitor::visitInitVal(BasicType *type, AST::Exp *exp) {
    int val = Evaluate::eval_const_exp(exp);
    return new InitialVal(type, new ConstantInt(val));
}

InitialExp *Visitor::visitInitExp(BasicType *type, AST::Exp *exp) {
    Value *val = visitExp(exp);
    return new InitialExp(type, val);
}

InitialArray *Visitor::visitInitArray(ArrayType *type, AST::InitArray *inits) {
    InitialArray *init_array = new InitialArray(type);

    if (type->base_type->is_basic_type()) {
        for (int i = 0; i < type->size; i++) {
            if (is_global) {
                init_array->add(visitInitVal(((BasicType *) (type->base_type)), (AST::Exp *) inits->inits[i]));
            } else {
                init_array->add(visitInitExp(((BasicType *) (type->base_type)), (AST::Exp *) inits->inits[i]));
            }
        }
    } else {
        for (int i = 0; i < type->size; i++) {
            init_array->add(visitInitArray((ArrayType *) (type->base_type), (AST::InitArray *) inits->inits[i]));
        }
    }

    return init_array;
}

Value *Visitor::visitExp(AST::Exp *exp) {
    if (auto *be = dynamic_cast<AST::BinaryExp *>(exp)) {
        return visitBinaryExp(be);
    } else if (auto *ue = dynamic_cast<AST::UnaryExp *>(exp)) {
        return visitUnaryExp(ue);
    } else {
        exit(90);
    }
}

Value *Visitor::visitBinaryExp(AST::BinaryExp *exp) {
    Value *first = visitExp(exp->first);
    for (int i = 0; i < exp->follows.size(); i++) {
        Token *op = exp->binary_ops[i];
        Value *second = visitExp(exp->follows[i]);
        if (op->is_alu_op()) {
            LLVM::Alu::AluOp alu_op = token_to_alu_op(op);
            ConstantInt *first_const = dynamic_cast<ConstantInt *>(first);
            ConstantInt *second_const = dynamic_cast<ConstantInt *> (second);
            if (first_const && second_const) {
                first = new ConstantInt(Evaluate::binary_cal(op, first_const->val, second_const->val));
            } else {
                first = new LLVM::Alu(new BasicType(TypeTag::int32_type),
                                      alu_op, first, second, cur_bb);
            }
        } else if (op->is_cmp_op()) {
            LLVM::Icmp::IcmpOp icmp_op = token_to_icmp_op(op);
            if (first->type->type_tag == TypeTag::int1_type) {
                first = new LLVM::Zext(first, new BasicType(TypeTag::int32_type), cur_bb);
            }
            if (second->type->type_tag == TypeTag::int1_type) {
                second = new LLVM::Zext(second, new BasicType(TypeTag::int32_type), cur_bb);
            }
            first = new LLVM::Icmp(icmp_op, first, second, cur_bb);
        } else {
            exit(92);
        }
    }
    return first;
}

Value *Visitor::visitUnaryExp(AST::UnaryExp *exp) {
    Value *primary_exp = visitPrimaryExp(exp->primary_exp);
    for (int i = exp->unary_ops.size() - 1; i >= 0; i--) {
        Token *op = exp->unary_ops[i];
        if (op->is_of(TokenType::MINU)) {
            if (auto *cv = dynamic_cast<ConstantInt *>(primary_exp)) {
                primary_exp = new ConstantInt(-cv->val);
            } else {
                primary_exp = new LLVM::Alu(new BasicType(TypeTag::int32_type),
                                            LLVM::Alu::AluOp::sub,
                                            new ConstantInt(0), primary_exp, cur_bb);
            }
        } else if (op->is_of(TokenType::NOT)) {
            primary_exp = new LLVM::Icmp(LLVM::Icmp::IcmpOp::eq, primary_exp, new ConstantInt(0), cur_bb);
            primary_exp = new LLVM::Zext(primary_exp, new BasicType(TypeTag::int32_type), cur_bb);
        }
    }
    return primary_exp;
}

Value *Visitor::visitPrimaryExp(AST::PrimaryExp *exp) {
    if (auto ep = dynamic_cast<AST::Exp *>(exp)) {
        return visitExp(ep);
    } else if (auto lp = dynamic_cast<AST::LVal *>(exp)) {
        return visitLVal(lp, true);
    } else if (auto np = dynamic_cast<AST::Number *>(exp)) {
        return visitNumber(np);
    } else if (auto cp = dynamic_cast<AST::Call *>(exp)) {
        return visitCall(cp);
    } else {
        exit(12);
    }
}

Value *Visitor::visitLVal(AST::LVal *lval, bool need_load) {
    std::string name = lval->ident->content;
    Symbol *symbol = cur_sym_table->get(name);
#ifdef ERROR_CHECK
    ErrorHandler::ERROR_HANDLER->handle_c(cur_sym_table, lval->ident);
    if (symbol == nullptr) return new ConstantInt(0);
#endif

//    std::cout << symbol->is_complete << " " << name << " " << lval->ident->line << " "
//              << std::string(*symbol->value->type) << std::endl;
    if (symbol->is_const && lval->indexes.empty()) {
        return ((InitialVal *) symbol->init)->value;
    }

    Value *value = symbol->value;

    std::vector<Value *> dims;
    if (symbol->is_complete)
        dims.push_back(new ConstantInt(0));
    for (AST::Exp *exp: lval->indexes) {
        dims.push_back(visitExp(exp));
    }

    if (symbol->is_complete) {
        Type *get_element_type = type_decay(value, dims.size() - 1);
        if (dims.size() > 1) {
            value = new LLVM::GetElementPtr(get_element_type, value, dims, cur_bb);
        }
        if (need_load) {
            if (((PointerType *) get_element_type)->ref_type->is_basic_type()) {
                value = new LLVM::Load(value, cur_bb);
            } else {
                std::vector<int> dims = {0, 0};
                value = new LLVM::GetElementPtr(type_decay(value, 1), value, dims, cur_bb);
            }
        }
    } else {
        if (dims.size() == 0) {
            if (need_load)
                value = new LLVM::Load(value, cur_bb);
        } else {
            value = new LLVM::Load(value, cur_bb);
            Type *get_element_type = type_decay(value, dims.size() - 1);
            value = new LLVM::GetElementPtr(get_element_type, value, dims, cur_bb);
            if (need_load) {
                if (((PointerType *) get_element_type)->ref_type->is_basic_type()) {
                    value = new LLVM::Load(value, cur_bb);
                } else {
                    std::vector<int> dims = {0, 0};
                    value = new LLVM::GetElementPtr(type_decay(value, 1), value, dims, cur_bb);
                }
            }
        }

    }

    return value;

}

Type *Visitor::type_decay(Value *value, int x) {
    Type *get_element_type = value->type;
    for (int i = 0; i < x; i++) {
        PointerType *ptype = dynamic_cast<PointerType *>(get_element_type);
        Type *ref_type = ptype->ref_type;
        if (ref_type->is_array_type()) {
            ArrayType *atype = dynamic_cast<ArrayType *>(ref_type);
            get_element_type = new PointerType(atype->base_type);
        } else {
            get_element_type = new PointerType(ref_type);
        }
    }
    return get_element_type;
}

Value *Visitor::visitNumber(AST::Number *num) {
    return new ConstantInt(Evaluate::eval_number(num));
}

Value *Visitor::visitCall(AST::Call *call) {
    std::string func_name = call->ident->content;

    Function *func = IR->functions['@' + func_name];
    std::vector<Value *> rparams;
    for (AST::Exp *exp: call->rparams) {
        rparams.push_back(visitExp(exp));
    }

#ifdef ERROR_CHECK
    ErrorHandler::ERROR_HANDLER->handle_c(cur_sym_table, call->ident);
    ErrorHandler::ERROR_HANDLER->handle_d(call->ident, rparams.size());
    ErrorHandler::ERROR_HANDLER->handle_e(call->ident, rparams);
    if (func == nullptr) return new ConstantInt(0);
#endif
    LLVM::Call *call_instr = new LLVM::Call(func, func->ret_type, rparams, cur_bb);
    return call_instr;
}

LLVM::Alu::AluOp Visitor::token_to_alu_op(Token *op) {
    if (op->is_of(TokenType::PLUS)) return LLVM::Alu::AluOp::add;
    else if (op->is_of(TokenType::MINU)) return LLVM::Alu::AluOp::sub;
    else if (op->is_of(TokenType::MULT)) return LLVM::Alu::AluOp::mul;
    else if (op->is_of(TokenType::DIV)) return LLVM::Alu::AluOp::sdiv;
    else if (op->is_of(TokenType::MOD)) return LLVM::Alu::AluOp::srem;
    else exit(91);
}

LLVM::Icmp::IcmpOp Visitor::token_to_icmp_op(Token *op) {
    if (op->is_of(TokenType::LSS)) return LLVM::Icmp::IcmpOp::slt;
    else if (op->is_of(TokenType::LEQ)) return LLVM::Icmp::IcmpOp::sle;
    else if (op->is_of(TokenType::GRE)) return LLVM::Icmp::IcmpOp::sgt;
    else if (op->is_of(TokenType::GEQ)) return LLVM::Icmp::IcmpOp::sge;
    else if (op->is_of(TokenType::EQL)) return LLVM::Icmp::IcmpOp::eq;
    else if (op->is_of(TokenType::NEQ)) return LLVM::Icmp::IcmpOp::ne;
    else exit(92);
}

void Visitor::traverse(std::vector<std::vector<int>> &ans, std::vector<int> &cur, std::vector<int> &dims, int depth) {
    if (depth == dims.size()) {
        ans.push_back(cur);
        return;
    }
    for (int i = 0; i < dims[depth]; i++) {
        cur.push_back(i);
        traverse(ans, cur, dims, depth + 1);
        cur.pop_back();
    }
}

Value *Visitor::trim_to(Value *value) {
    if (value->type->type_tag == TypeTag::int32_type) {
        return new LLVM::Icmp(LLVM::Icmp::IcmpOp::ne,
                              value, new ConstantInt(0), cur_bb);
    }
    return value;
}
