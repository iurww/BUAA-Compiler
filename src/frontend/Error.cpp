//
// Created by wwr on 2023/11/5.
//

#include "../../include/frontend/Error.h"
#include "../../include/frontend/Symbol.h"
#include "../../include/frontend/Token.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/IRModule.h"
#include "../../include/mir/Type.h"

ErrorHandler *ErrorHandler::ERROR_HANDLER = new ErrorHandler();

void ErrorHandler::handle_a(Token *fs) {
    std::string &s = fs->content;
    bool flag = false;
    for (int i = 1; i < s.size() - 1; i++) {
        if (s[i] == '%') {
            if (s[i + 1] != 'd')
                flag = true;
        } else if (!(s[i] == 32 || s[i] == 33 || (s[i] >= 40 && s[i] <= 126))) {
            flag = true;
        } else if (s[i] == '\\' && s[i + 1] != 'n') {
            flag = true;
        }
    }
    if (flag) {
        handle_error(fs->line, 'a');
    }
}

bool ErrorHandler::handle_b(SymbolTable *cur_sym_table, Token *name) {
    std::string &s = name->content;
    if (cur_sym_table->symbol_map.count(s)) {
        handle_error(name->line, 'b');
        return true;
    }
    return false;
}

void ErrorHandler::handle_c(SymbolTable *cur_sym_table, Token *name) {
    std::string &s = name->content;
    if (cur_sym_table->get(s) == nullptr) {
        handle_error(name->line, 'c');
    }
}

void ErrorHandler::handle_d(Token *name, int rparam_cnt) {
    Function *func = IRModule::IR_MODULE->functions['@' + name->content];
    if (func == nullptr) return;
    if (func->params.size() != rparam_cnt) {
        std::cout << func->params.size() << " " << rparam_cnt << "\n";
        std::cout << *func << "\n";

        handle_error(name->line, 'd');
    }
}

void ErrorHandler::handle_e(Token *name, std::vector<Value *> &rparams) {
    Function *func = IRModule::IR_MODULE->functions['@' + name->content];
    if (func == nullptr) return;
    if (func->params.size() != rparams.size()) return;
    bool flag = false;
    for (int i = 0; i < func->params.size(); i++) {
        if (*func->params[i]->type != *rparams[i]->type) {
            flag = true;
            break;
        }
    }
    if (flag) {
        handle_error(name->line, 'e');
    }
}

void ErrorHandler::handle_f(Function *cur_func, Token *ret, void *exp) {
    if (cur_func->ret_type->is_void_type() && exp != nullptr) {
        handle_error(ret->line, 'f');
    }
}

void ErrorHandler::handle_g(int line) {
    handle_error(line, 'g');
}

void ErrorHandler::handle_h(SymbolTable *cur_sym_table, Token *name) {
    Symbol *symbol = cur_sym_table->get(name->content);
    if (symbol && symbol->is_const) {
        handle_error(name->line, 'h');
    }
}

void ErrorHandler::handle_i(int line) {
    handle_error(line, 'i');
}

void ErrorHandler::handle_j(int line) {
    handle_error(line, 'j');
}

void ErrorHandler::handle_k(int line) {
    handle_error(line, 'k');
}

void ErrorHandler::handle_l(Token *fs, int param_cnt) {
    int cnt = 0;
    for (int i = 1; i < fs->content.size() - 1; i++) {
        if (fs->content[i] == '%' && fs->content[i + 1] == 'd') cnt++;
    }
    if (cnt != param_cnt) {
        handle_error(fs->line, 'l');
    }
}

void ErrorHandler::handle_m(Token *token) {
    handle_error(token->line, 'm');
}