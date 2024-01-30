//
// Created by wwr on 2023/11/6.
//

#ifndef COMPILER_TOKEN_H
#define COMPILER_TOKEN_H

#include <iostream>
#include <unordered_map>
#include <vector>

enum class TokenType {
    IDENFR = 0,
    INTCON,
    STRCON,
    MAINTK,
    CONSTTK,
    INTTK,
    BREAKTK,
    CONTINUETK,
    IFTK,
    ELSETK,
    NOT,
    AND,
    OR,
    FORTK,
    GETINTTK,
    PRINTFTK,
    RETURNTK,
    PLUS,
    MINU,
    VOIDTK,
    MULT,
    DIV,
    MOD,
    LSS,
    LEQ,
    GRE,
    GEQ,
    EQL,
    NEQ,
    ASSIGN,
    SEMICN,
    COMMA,
    LPARENT,
    RPARENT,
    LBRACK,
    RBRACK,
    LBRACE,
    RBRACE,
    UNDEFINED,
};

extern std::unordered_map<std::string, TokenType> operator_to_token;

extern std::unordered_map<std::string, TokenType> reserve_to_token;

extern std::unordered_map<TokenType, std::string> token_name;

class Token {
public:
    TokenType token_type;
    std::string content;
    int line;

    Token(TokenType type, std::string cont, int line) : token_type(type), content(cont), line(line) {
    }

    bool is_of(TokenType type) {
        return token_type == type;
    }

    bool is_basic_type() {
        return token_type == TokenType::INTTK;
    }

    bool is_func_type() {
        return token_type == TokenType::INTTK || token_type == TokenType::VOIDTK;
    }

    bool is_numeric_const() {
        return token_type == TokenType::INTCON;
    }

    bool is_alu_op() {
        return token_type == TokenType::PLUS || token_type == TokenType::MINU ||
               token_type == TokenType::MULT || token_type == TokenType::DIV ||
               token_type == TokenType::MOD;
    }

    bool is_cmp_op() {
        return token_type >= TokenType::LSS && token_type <= TokenType::NEQ;
    }

    bool is_unary_op() {
        return token_type == TokenType::PLUS || token_type == TokenType::MINU ||
               token_type == TokenType::NOT;
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &token) {
//        os << token_name[token.token_type] << " " << token.content << " " << token.line << std::endl;
        os << token_name[token.token_type] << " " << token.content;
        return os;
    }
};

class TokenList {
public:
    std::vector<Token *> tokens;

    TokenList() = default;

    void append(Token *token) {
        tokens.push_back(token);
    }

    friend std::ostream &operator<<(std::ostream &os, const TokenList &token_list) {
        for (Token *token: token_list.tokens) {
            os << *token << std::endl;
        }
        return os;
    }

};


#endif //COMPILER_TOKEN_H
