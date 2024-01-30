//
// Created by wwr on 2023/12/12.
//

#ifndef COMPILER_DFG_H
#define COMPILER_DFG_H

#include <unordered_map>
#include <string>

class Function;

class DFG {
public:
    std::unordered_map<std::string, Function *> &functions;

    DFG(std::unordered_map<std::string, Function *> &functions);

    void run();

    void make_cfg();

    void make_dom();
    void make_func_dom(Function *func);
    void make_func_sdom(Function *func);
    void make_func_idom(Function *func);
    void make_func_df(Function *func);

    void make_def_use();
    void make_func_def_use(Function *func);
};


#endif //COMPILER_DFG_H
