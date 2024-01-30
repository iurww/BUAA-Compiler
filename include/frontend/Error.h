//
// Created by wwr on 2023/11/5.
//

#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

#include <vector>
#include <iostream>

class Token;

class SymbolTable;

class Function;

class Value;

class ErrorHandler {
public:
    static ErrorHandler *ERROR_HANDLER;

    std::vector<std::pair<int, char>> errors;

    ErrorHandler() = default;

    ~ErrorHandler() = default;

    void handle_error(int line, char error_type) {
        std::cout << line << " " << error_type << std::endl;
        errors.push_back({line, error_type});
    }

    void handle_a(Token *fs);

    bool handle_b(SymbolTable *cur_sym_table, Token *name);

    void handle_c(SymbolTable *cur_sym_table, Token *name);

    void handle_d(Token *name, int rparam_cnt);

    void handle_e(Token *name, std::vector<Value *> &rparams);

    void handle_f(Function *cur_func, Token *ret, void *exp);

    void handle_g(int line);

    void handle_h(SymbolTable *cur_sym_table, Token *name);

    void handle_i(int line);

    void handle_j(int line);

    void handle_k(int line);

    void handle_l(Token *fs, int param_cnt);

    void handle_m(Token *token);

};


#endif //COMPILER_ERROR_H
