//
// Created by wwr on 2023/10/9.
//

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "AST.h"
#include <sstream>

class Parser {
public:
    TokenList &token_list;
    int index;
    std::vector<std::string> result;

    bool has_next() {
        return index < token_list.tokens.size();
    }

    Token *peek(int cnt) {
        return token_list.tokens[index + cnt];
    }

    Token *get_token() {
        add_result(token_list.tokens[index]);
        return token_list.tokens[index++];
    }

    Token *get_token_expect(TokenType type) {
        Token *token = token_list.tokens[index];
        if (token->token_type == type) {
            add_result(token_list.tokens[index]);
            index++;
            return token;
        } else {
            std::cout << index << " " << *token << std::endl;
            exit(1);
        }

    }

    Parser(TokenList &t) : token_list(t), index(0) {

    }

    void add_result(std::string s) {
        result.push_back(s);
    }

    void add_result(Token *t) {
        std::ostringstream oss;
        oss << *t;
        result.push_back(oss.str());
    }

    enum BinaryExpType {
        LOR,
        LAND,
        EQ,
        REL,
        ADD,
        MUL,
    };

    void add_binary_result(BinaryExpType type) {
        switch (type) {
            case MUL:
                add_result("<MulExp>");
                break;
            case ADD:
                add_result("<AddExp>");
                break;
            case REL:
                add_result("<RelExp>");
                break;
            case EQ:
                add_result("<EqExp>");
                break;
            case LAND:
                add_result("<LAndExp>");
                break;
            case LOR:
                add_result("<LOrExp>");
                break;
        }
    }

    /**
     * CompUnit → {Decl} {FuncDef} MainFuncDef
     */
    AST::CompUnit *parseCompUnit();

    /**
     * Decl → ConstDecl | VarDecl
     */
    AST::Decl *parseDecl();

    /**
     * ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
     * BType → 'int'
     */
    AST::Decl *parseConstDecl();

    /**
     * ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
     */
    AST::Def *parseConstDef(Token *type);

    /**
     * VarDecl → BType VarDef { ',' VarDef } ';'
     */
    AST::Decl *parseVarDecl();

    /**
     * VarDef → Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
     */
    AST::Def *parseVarDef(Token *type);

    /**
     * ConstInitVal → ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
     */
    AST::Init *parseConstInitVal();

    /**
     * InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'
     */
    AST::Init *parseInitVal();

    /**
     * MainFuncDef → 'int' 'main' '(' ')' Block
     */
    AST::FuncDef *parseMainFuncDef();

    /**
     * FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
     */
    AST::FuncDef *parseFuncDef();

    /**
     * FuncType → 'void' | 'int'
     */
    Token *parseFuncType();

    /**
     * FuncFParams → FuncFParam { ',' FuncFParam }
     */
    std::vector<AST::FuncFParam *> parseFuncFParams();

    /**
     * FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
     */
    AST::FuncFParam *parseFuncFParam();

    /**
     * Block → '{' { BlockItem } '}'
     */
    AST::Block *parseBlock();

    /**
     * BlockItem → Decl | Stmt
     */
    AST::BlockItem *parseBlockItem();

    /**
     * Stmt →
     * LVal '=' Exp ';'  |
     * [Exp] ';' |
     * Block |
     * 'if' '(' Cond ')' Stmt [ 'else' Stmt ] |
     * 'for' '(' [ForStmt] ';' [Cond] ';' [forStmt] ')' Stmt |
     * 'break' ';' | 'continue' ';' |
     * 'return' [Exp] ';' |
     * LVal '=' 'getint''('')'';' |
     * 'printf''('FormatString{','Exp}')'';'
     */
    AST::Stmt *parseStmt();

    /**
     * ForStmt → LVal '=' Exp
     */
    AST::AssignStmt *parseForStmt();

    /**
     * LVal → Ident {'[' Exp ']'}
     */
    AST::LVal *parseLVal();

    /**
     * PrimaryExp → '(' Exp ')' | LVal | Number
     */
    AST::PrimaryExp *parsePrimaryExp();

    /**
     * Number → IntConst
     */
    AST::Number *parseNumber();

    /**
     * Call → Ident '(' [FuncRParams] ')'
     */
    AST::Call *parseCall();

    /**
     * FuncRParams → Exp { ',' Exp }
     */
    std::vector<AST::Exp *> parseFuncRParams();

    /**
     * UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
     */
    AST::UnaryExp *parseUnaryExp();

    AST::Exp *parseSubBinaryExp(BinaryExpType exp_type);

    bool isSubBinaryOp(BinaryExpType exp_type, TokenType token_type);

    /**
     * LOrExp → LAndExp | LOrExp '||' LAndExp
     * LAndExp → EqExp | LAndExp '&&' EqExp
     * EqExp → RelExp | EqExp ('==' | '!=') RelExp
     * RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
     * AddExp → MulExp | AddExp ('+' | '−') MulExp
     * MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
     * @param exp_type 二元表达式类型
     */
    AST::BinaryExp *parseBinaryExp(BinaryExpType exp_type);

    /**
     * Cond → LOrExp
     */
    AST::BinaryExp *parseCond();

    /**
     * ConstExp → AddExp 注：使用的Ident必须是常量
     */
    AST::BinaryExp *parseConstExp();

    /**
     * Exp → AddExp
     */
    AST::BinaryExp *parseExp();
};

#endif //COMPILER_PARSER_H
