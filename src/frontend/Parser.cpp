//
// Created by wwr on 2023/10/9.
//

#include "../../include/frontend/Parser.h"
#include "../../include/util/config.h"
#include "Error.h"

AST::CompUnit *Parser::parseCompUnit() {
    AST::CompUnit *root = new AST::CompUnit();
    while (has_next()) {
        if (peek(2)->is_of(TokenType::LPARENT)) {
            if (peek(1)->is_of(TokenType::MAINTK))
                root->func_defs.push_back(parseMainFuncDef());
            else
                root->func_defs.push_back(parseFuncDef());
        } else {
            root->decls.push_back(parseDecl());
        }
    }
    add_result("<CompUnit>");
    return root;
}


AST::Decl *Parser::parseDecl() {
    if (peek(0)->is_of(TokenType::CONSTTK))
        return parseConstDecl();
    else
        return parseVarDecl();
}


AST::Decl *Parser::parseConstDecl() {
    AST::Decl *decl = new AST::Decl();
    decl->is_const = true;
    get_token_expect(TokenType::CONSTTK);
    Token *type = get_token();
    decl->type = type;
    decl->defs.push_back(parseConstDef(type));
    while (peek(0)->is_of(TokenType::COMMA)) {
        get_token();
        decl->defs.push_back(parseConstDef(type));
    }
#ifdef ERROR_CHECK
    if (!peek(0)->is_of(TokenType::SEMICN))
        ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
    else
        get_token_expect(TokenType::SEMICN);
#else
    get_token();
#endif
    add_result("<ConstDecl>");
    return decl;
}

AST::Def *Parser::parseConstDef(Token *type) {
    AST::Def *def = new AST::Def();
    def->type = type;
    def->ident = get_token_expect(TokenType::IDENFR);
    while (peek(0)->is_of(TokenType::LBRACK)) {
        get_token();
        def->sizes.push_back(parseConstExp());
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::RBRACK))
            ErrorHandler::ERROR_HANDLER->handle_k(peek(-1)->line);
        else
            get_token_expect(TokenType::RBRACK);
#else
        get_token();
#endif
    }
    if (peek(0)->is_of(TokenType::ASSIGN)) get_token();
    def->init = parseConstInitVal();
    add_result("<ConstDef>");
    return def;
}

AST::Decl *Parser::parseVarDecl() {
    AST::Decl *decl = new AST::Decl();
    decl->is_const = false;
    Token *type = get_token();
    decl->type = type;
    decl->defs.push_back(parseVarDef(type));
    while (peek(0)->is_of(TokenType::COMMA)) {
        get_token();
        decl->defs.push_back(parseVarDef(type));
    }
#ifdef ERROR_CHECK
    if (!peek(0)->is_of(TokenType::SEMICN))
        ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
    else
        get_token_expect(TokenType::SEMICN);
#else
    get_token();
#endif
    add_result("<VarDecl>");
    return decl;
}

AST::Def *Parser::parseVarDef(Token *type) {
    AST::Def *def = new AST::Def();
    def->type = type;
    def->ident = get_token_expect(TokenType::IDENFR);
    while (peek(0)->is_of(TokenType::LBRACK)) {
        get_token();
        def->sizes.push_back(parseConstExp());
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::RBRACK))
            ErrorHandler::ERROR_HANDLER->handle_k(peek(-1)->line);
        else
            get_token_expect(TokenType::RBRACK);
#else
        get_token();
#endif
    }
    if (peek(0)->is_of(TokenType::ASSIGN)) {
        get_token();
        def->init = parseInitVal();
    }
    add_result("<VarDef>");
    return def;
}

AST::Init *Parser::parseConstInitVal() {
    AST::Init *init = nullptr;
    if (peek(0)->is_of(TokenType::LBRACE)) {
        AST::InitArray *init_val = new AST::InitArray();
        get_token();
        init_val->inits.push_back(parseConstInitVal());
        while (peek(0)->is_of(TokenType::COMMA)) {
            get_token();
            init_val->inits.push_back(parseConstInitVal());
        }
        if (peek(0)->is_of(TokenType::RBRACE)) get_token();
        init = dynamic_cast<AST::Init *>(init_val);
    } else {
        AST::Exp *init_val = parseConstExp();
        init = dynamic_cast<AST::Init *>(init_val);
    }
    add_result("<ConstInitVal>");
    return init;
}

AST::Init *Parser::parseInitVal() {
    AST::Init *init = nullptr;
    if (peek(0)->is_of(TokenType::LBRACE)) {
        AST::InitArray *init_val = new AST::InitArray();
        get_token();
        init_val->inits.push_back(parseInitVal());
        while (peek(0)->is_of(TokenType::COMMA)) {
            get_token();
            init_val->inits.push_back(parseInitVal());
        }
        if (peek(0)->is_of(TokenType::RBRACE)) get_token();
        init = dynamic_cast<AST::Init *>(init_val);
    } else {
        AST::Exp *init_val = parseExp();
        init = dynamic_cast<AST::Init *>(init_val);
    }
    add_result("<InitVal>");
    return init;
}

AST::FuncDef *Parser::parseMainFuncDef() {
    AST::FuncDef *func = new AST::FuncDef();
    func->type = get_token();
    func->ident = get_token_expect(TokenType::MAINTK);
    get_token();
#ifdef ERROR_CHECK
    if (!peek(0)->is_of(TokenType::RPARENT))
        ErrorHandler::ERROR_HANDLER->handle_j(peek(-1)->line);
    else
        get_token_expect(TokenType::RPARENT);
#else
    get_token();
#endif
    func->body = parseBlock();
    add_result("<MainFuncDef>");
    return func;
}

AST::FuncDef *Parser::parseFuncDef() {
    AST::FuncDef *func = new AST::FuncDef();
    func->type = parseFuncType();
    func->ident = get_token_expect(TokenType::IDENFR);
    get_token();
    if (peek(0)->is_basic_type()) {
        func->fparams = parseFuncFParams();
    }
#ifdef ERROR_CHECK
    if (!peek(0)->is_of(TokenType::RPARENT))
        ErrorHandler::ERROR_HANDLER->handle_j(peek(-1)->line);
    else
        get_token_expect(TokenType::RPARENT);
#else
    get_token();
#endif
    func->body = parseBlock();
    add_result("<FuncDef>");
    return func;
}

Token *Parser::parseFuncType() {
    if (peek(0)->is_func_type()) {
        Token *type = get_token();
        add_result("<FuncType>");
        return type;
    }
    return nullptr;
}

std::vector<AST::FuncFParam *> Parser::parseFuncFParams() {
    std::vector<AST::FuncFParam *> fparams;
    fparams.push_back(parseFuncFParam());
    while (peek(0)->is_of(TokenType::COMMA)) {
        get_token();
        fparams.push_back(parseFuncFParam());
    }
    add_result("<FuncFParams>");
    return fparams;
}

AST::FuncFParam *Parser::parseFuncFParam() {
    AST::FuncFParam *fparam = new AST::FuncFParam();
    fparam->type = get_token();
    fparam->ident = get_token_expect(TokenType::IDENFR);
    if (peek(0)->is_of(TokenType::LBRACK)) {
        fparam->is_array = true;
        get_token();
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::RBRACK))
            ErrorHandler::ERROR_HANDLER->handle_k(peek(-1)->line);
        else
            get_token_expect(TokenType::RBRACK);
#else
        get_token();
#endif
        while (peek(0)->is_of(TokenType::LBRACK)) {
            get_token();
            fparam->sizes.push_back(parseConstExp());
#ifdef ERROR_CHECK
            if (!peek(0)->is_of(TokenType::RBRACK))
                ErrorHandler::ERROR_HANDLER->handle_k(peek(-1)->line);
            else
                get_token_expect(TokenType::RBRACK);
#else
            get_token();
#endif
        }
    }
    add_result("<FuncFParam>");
    return fparam;
}

AST::Block *Parser::parseBlock() {
    AST::Block *block = new AST::Block();
    get_token();
    while (!peek(0)->is_of(TokenType::RBRACE)) {
        block->items.push_back(parseBlockItem());
    }
    block->end_rbrace = get_token();
    add_result("<Block>");
    return block;
}

AST::BlockItem *Parser::parseBlockItem() {
    if (peek(0)->is_of(TokenType::CONSTTK)
        || peek(0)->is_basic_type()) {
        return parseDecl();
    } else {
        return parseStmt();
    }
}

AST::Stmt *Parser::parseStmt() {
    AST::Stmt *stmt = nullptr;
    Token *temp = peek(0);
    if (temp->is_of(TokenType::LBRACE)) {
        stmt = parseBlock();
    } else if (temp->is_of(TokenType::IFTK)) {
        AST::IfStmt *if_stmt = new AST::IfStmt();
        get_token();
        get_token();
        if_stmt->cond = parseCond();
        get_token();
        if_stmt->then_stmt = parseStmt();
        if (peek(0)->is_of(TokenType::ELSETK)) {
            get_token();
            if_stmt->else_stmt = parseStmt();
        }
        stmt = if_stmt;
    } else if (temp->is_of(TokenType::FORTK)) {
        AST::ForStmt *for_stmt = new AST::ForStmt();
        get_token();
        get_token();

        for_stmt->exp1 = peek(0)->is_of(TokenType::IDENFR) ? parseForStmt() : nullptr;
        get_token_expect(TokenType::SEMICN);

        for_stmt->exp2 = !peek(0)->is_of(TokenType::SEMICN) ? parseCond() : nullptr;
        get_token_expect(TokenType::SEMICN);

        for_stmt->exp3 = peek(0)->is_of(TokenType::IDENFR) ? parseForStmt() : nullptr;
        get_token();

        for_stmt->body = parseStmt();
        stmt = for_stmt;
    } else if (temp->is_of(TokenType::BREAKTK)) {
        Token *break_token = get_token();
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::SEMICN))
            ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
        else
            get_token_expect(TokenType::SEMICN);
#else
        get_token();
#endif
        AST::BreakStmt *break_stmt = new AST::BreakStmt();
        break_stmt->break_token = break_token;
        stmt = break_stmt;
    } else if (temp->is_of(TokenType::CONTINUETK)) {
        Token *continue_token = get_token();
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::SEMICN))
            ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
        else
            get_token_expect(TokenType::SEMICN);
#else
        get_token();
#endif
        AST::ContinueStmt *continue_stmt = new AST::ContinueStmt();
        continue_stmt->continue_token = continue_token;
        stmt = continue_stmt;
    } else if (temp->is_of(TokenType::RETURNTK)) {
        AST::ReturnStmt *return_stmt = new AST::ReturnStmt();
        return_stmt->exp = nullptr;
        return_stmt->ret_token = temp;
        get_token();
        if (!peek(0)->is_of(TokenType::SEMICN)) {
            return_stmt->exp = parseExp();
        }
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::SEMICN))
            ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
        else
            get_token_expect(TokenType::SEMICN);
#else
        get_token();
#endif
        stmt = return_stmt;
    } else if (temp->is_of(TokenType::IDENFR)) {
        int cur_index = index;
        int cur_ans_index = result.size();
        AST::LVal *lval = parseLVal();
        if (peek(0)->is_of(TokenType::ASSIGN)) {
            AST::AssignStmt *assign = new AST::AssignStmt();
            assign->lval = lval;
            get_token();
            if (peek(0)->is_of(TokenType::GETINTTK)) {
                get_token();
                get_token();
#ifdef ERROR_CHECK
                if (!peek(0)->is_of(TokenType::RPARENT))
                    ErrorHandler::ERROR_HANDLER->handle_j(peek(-1)->line);
                else
                    get_token_expect(TokenType::RPARENT);
#else
                get_token();
#endif
#ifdef ERROR_CHECK
                if (!peek(0)->is_of(TokenType::SEMICN))
                ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
            else
                get_token_expect(TokenType::SEMICN);
#else
                get_token();
#endif
                assign->is_input = true;
                assign->exp = nullptr;
            } else {
                assign->is_input = false;
                assign->exp = parseExp();
#ifdef ERROR_CHECK
                if (!peek(0)->is_of(TokenType::SEMICN))
                ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
            else
                get_token_expect(TokenType::SEMICN);
#else
                get_token();
#endif
            }
            stmt = assign;
        } else {
            delete lval;
            index = cur_index;
            result.erase(result.begin() + cur_ans_index, result.end());
            AST::ExpStmt *exp_stmt = new AST::ExpStmt();
            exp_stmt->exp = parseExp();
#ifdef ERROR_CHECK
            if (!peek(0)->is_of(TokenType::SEMICN))
                ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
            else
                get_token_expect(TokenType::SEMICN);
#else
            get_token();
#endif
            stmt = exp_stmt;
        }
    } else if (temp->is_of(TokenType::PRINTFTK)) {
        AST::PrintfStmt *printf_stmt = new AST::PrintfStmt();
        get_token();
        get_token();
        printf_stmt->fs = get_token_expect(TokenType::STRCON);
#ifdef ERROR_CHECK
        ErrorHandler::ERROR_HANDLER->handle_a(printf_stmt->fs);
#endif
        while (peek(0)->is_of(TokenType::COMMA)) {
            get_token();
            printf_stmt->params.push_back(parseExp());
        }
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::RPARENT))
            ErrorHandler::ERROR_HANDLER->handle_j(peek(-1)->line);
        else
            get_token_expect(TokenType::RPARENT);
#else
        get_token();
#endif

#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::SEMICN))
            ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
        else
            get_token_expect(TokenType::SEMICN);
#else
        get_token();
#endif
        stmt = printf_stmt;
    } else if (temp->is_of(TokenType::SEMICN)) {
        get_token();
        stmt = new AST::ExpStmt();
    } else {
        AST::ExpStmt *exp_stmt = new AST::ExpStmt();
        exp_stmt->exp = parseExp();
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::SEMICN))
            ErrorHandler::ERROR_HANDLER->handle_i(peek(-1)->line);
        else
            get_token_expect(TokenType::SEMICN);
#else
        get_token();
#endif
        stmt = exp_stmt;
    }
    add_result("<Stmt>");
    return stmt;
}

AST::AssignStmt *Parser::parseForStmt() {
    AST::AssignStmt *assign = new AST::AssignStmt();
    assign->is_input = false;
    assign->lval = parseLVal();
    get_token_expect(TokenType::ASSIGN);
    assign->exp = parseExp();
    add_result("<ForStmt>");
    return assign;
}

AST::LVal *Parser::parseLVal() {
    AST::LVal *lval = new AST::LVal();
    lval->ident = get_token_expect(TokenType::IDENFR);
    while (peek(0)->is_of(TokenType::LBRACK)) {
        get_token();
        lval->indexes.push_back(parseExp());
#ifdef ERROR_CHECK
        if (!peek(0)->is_of(TokenType::RBRACK))
            ErrorHandler::ERROR_HANDLER->handle_k(peek(-1)->line);
        else
            get_token_expect(TokenType::RBRACK);
#else
        get_token();
#endif
    }
    add_result("<LVal>");
    return lval;
}

AST::PrimaryExp *Parser::parsePrimaryExp() {
    AST::PrimaryExp *primary = nullptr;
    if (peek(0)->is_of(TokenType::LPARENT)) {
        get_token_expect(TokenType::LPARENT);
        primary = parseExp();
        get_token_expect(TokenType::RPARENT);
    } else if (peek(0)->is_of(TokenType::IDENFR)) {
        primary = parseLVal();
    } else if (peek(0)->is_numeric_const()) {
        primary = parseNumber();
    }
    add_result("<PrimaryExp>");
    return primary;
}

AST::Number *Parser::parseNumber() {
    AST::Number *number = new AST::Number();
    number->token = get_token();
    add_result("<Number>");
    return number;
}

AST::Call *Parser::parseCall() {
    AST::Call *call = new AST::Call();
    call->ident = get_token_expect(TokenType::IDENFR);
    get_token_expect(TokenType::LPARENT);
    if (peek(0)->is_of(TokenType::LPARENT) ||
        peek(0)->is_of(TokenType::IDENFR) ||
        peek(0)->is_of(TokenType::INTCON) ||
        peek(0)->is_unary_op()) {
        call->rparams = parseFuncRParams();
    }
#ifdef ERROR_CHECK
    if (!peek(0)->is_of(TokenType::RPARENT))
        ErrorHandler::ERROR_HANDLER->handle_j(peek(-1)->line);
    else
        get_token_expect(TokenType::RPARENT);
#else
    get_token();
#endif
    return call;
}

std::vector<AST::Exp *> Parser::parseFuncRParams() {
    std::vector<AST::Exp *> rparams;
    rparams.push_back(parseExp());
    while (peek(0)->is_of(TokenType::COMMA)) {
        get_token();
        rparams.push_back(parseExp());
    }
    add_result("<FuncRParams>");
    return rparams;
}

AST::UnaryExp *Parser::parseUnaryExp() {
    AST::UnaryExp *unary = new AST::UnaryExp();
    while (peek(0)->is_of(TokenType::PLUS)
           || peek(0)->is_of(TokenType::MINU)
           || peek(0)->is_of(TokenType::NOT)) {
        unary->unary_ops.push_back(get_token());
        add_result("<UnaryOp>");
    }
    if (peek(0)->is_of(TokenType::IDENFR) &&
        peek(1)->is_of(TokenType::LPARENT)) {
        unary->primary_exp = parseCall();
    } else {
        unary->primary_exp = parsePrimaryExp();
    }
    for (int i = 1; i <= unary->unary_ops.size(); i++) add_result("<UnaryExp>");
    add_result("<UnaryExp>");
    return unary;
}

AST::Exp *Parser::parseSubBinaryExp(BinaryExpType exp_type) {
    if (exp_type == LOR) {
        return parseBinaryExp(BinaryExpType::LAND);
    } else if (exp_type == LAND) {
        return parseBinaryExp(BinaryExpType::EQ);
    } else if (exp_type == EQ) {
        return parseBinaryExp(BinaryExpType::REL);
    } else if (exp_type == REL) {
        return parseBinaryExp(BinaryExpType::ADD);
    } else if (exp_type == ADD) {
        return parseBinaryExp(BinaryExpType::MUL);
    } else if (exp_type == MUL) {
        return parseUnaryExp();
    } else {
        exit(2);
    }
}

bool Parser::isSubBinaryOp(BinaryExpType exp_type, TokenType token_type) {
    if (exp_type == LOR) {
        return token_type == TokenType::OR;
    } else if (exp_type == LAND) {
        return token_type == TokenType::AND;
    } else if (exp_type == EQ) {
        return token_type == TokenType::EQL || token_type == TokenType::NEQ;
    } else if (exp_type == REL) {
        return token_type == TokenType::LSS || token_type == TokenType::GRE ||
               token_type == TokenType::LEQ || token_type == TokenType::GEQ;
    } else if (exp_type == ADD) {
        return token_type == TokenType::PLUS || token_type == TokenType::MINU;
    } else if (exp_type == MUL) {
        return token_type == TokenType::MULT || token_type == TokenType::DIV ||
               token_type == TokenType::MOD;
    } else {
        exit(41);
    }
}

AST::BinaryExp *Parser::parseBinaryExp(BinaryExpType exp_type) {
    AST::BinaryExp *binary = new AST::BinaryExp();
    binary->first = parseSubBinaryExp(exp_type);
    while (has_next() && isSubBinaryOp(exp_type, peek(0)->token_type)) {
        add_binary_result(exp_type);
        binary->binary_ops.push_back(get_token());
        binary->follows.push_back(parseSubBinaryExp(exp_type));
    }
    add_binary_result(exp_type);
    return binary;
}

AST::BinaryExp *Parser::parseCond() {
    AST::BinaryExp *binary = parseBinaryExp(BinaryExpType::LOR);
    add_result("<Cond>");
    return binary;
}


AST::BinaryExp *Parser::parseConstExp() {
    AST::BinaryExp *binary = parseBinaryExp(BinaryExpType::ADD);
    add_result("<ConstExp>");
    return binary;
}


AST::BinaryExp *Parser::parseExp() {
    AST::BinaryExp *binary = parseBinaryExp(BinaryExpType::ADD);
    add_result("<Exp>");
    return binary;
}