//
// Created by wwr on 2023/12/12.
//

#include "../../include/midend/DFG.h"
#include "../../include/mir/Function.h"
#include "../../include/mir/BasicBlock.h"
#include "../../include/mir/LLVMInstr.h"
#include "../../include/mir/Type.h"
#include <algorithm>
#include <iterator>
#include <iostream>

DFG::DFG(std::unordered_map<std::string, Function *> &functions) : functions(functions) {

}

void DFG::run() {
    make_cfg();
    make_def_use();
    make_dom();
}

void DFG::make_cfg() {
    for (auto it: functions) {
        for (auto *bb: it.second->blocks) {
            LLVMInstr *instr = bb->instructions[bb->instructions.size() - 1];
            if (auto *br = dynamic_cast<LLVM::Branch *>(instr)) {
                br->bb->next.push_back(br->true_block);
                br->bb->next.push_back(br->false_block);
                br->true_block->prev.push_back(br->bb);
                br->false_block->prev.push_back(br->bb);
            } else if (auto *j = dynamic_cast<LLVM::Jump *>(instr)) {
                j->bb->next.push_back(j->dst);
                j->dst->prev.push_back(j->bb);
            }
        }
    }
}

void DFG::make_dom() {
    for (auto it: functions) {
        std::set<BasicBlock *> all(it.second->blocks.begin(), it.second->blocks.end());
        BasicBlock *entry = it.second->blocks[0];
        entry->dom_by.insert(entry);
        for (int i = 1; i < it.second->blocks.size(); i++) {
            it.second->blocks[i]->dom_by = all;
        }
        make_func_dom(it.second);
        make_func_sdom(it.second);
        make_func_idom(it.second);
        make_func_df(it.second);
//        for (auto *bb: it.second->blocks) {
//            std::cout << bb->name << " : ";
//            for (auto *dom: bb->df) {
//                std::cout << dom->name << " ";
//            }
//            std::cout << std::endl;
//        }
    }
}

void DFG::make_func_dom(Function *func) {
    bool flag;
    do {
        flag = false;
        for (auto *bb: func->blocks) {
            std::set<BasicBlock *> new_dom_by;
            int cnt = 0;
            for (auto *prev: bb->prev) {
                if (cnt == 0) {
                    new_dom_by = prev->dom_by;
                } else {
                    std::set_intersection(
                            new_dom_by.begin(), new_dom_by.end(),
                            prev->dom_by.begin(), prev->dom_by.end(),
                            std::inserter(new_dom_by, new_dom_by.begin()));
                }
                cnt++;
            }
            new_dom_by.insert(bb);
            if (bb->dom_by != new_dom_by) {
                bb->dom_by = new_dom_by;
                flag = true;
            }
        }
    } while (flag);
    for (auto *bb : func->blocks) {
        for (auto dom : bb->dom_by) {
            dom->doms.insert(bb);
        }
    }
}

void DFG::make_func_sdom(Function *func) {
    for (auto *bb : func->blocks) {
        bb->sdom_by = bb->dom_by;
        bb->sdom_by.erase(bb);
    }
    for (auto *bb : func->blocks) {
        for (auto sdom : bb->sdom_by) {
            sdom->sdoms.insert(bb);
        }
    }
}

void DFG::make_func_idom(Function *func) {
    for (auto *bb : func->blocks) {
        for (auto *sdom : bb->sdom_by) {
            bool flag = false;
            for (auto *sdom_sdom : sdom->sdoms) {
                if (sdom_sdom->sdoms.count(bb)) flag= true;
            }
            if (!flag) {
                bb->idom_by = sdom;
                sdom->idoms.insert(bb);
            }
        }
    }
}

void DFG::make_func_df(Function *func) {
    for (auto *bb : func->blocks) {
        for (auto *next_bb : bb->next) {
            BasicBlock *x = bb;
            while (!x->sdoms.count(next_bb)) {
                x->df.insert(next_bb);
                if (x->idom_by != nullptr) {
                    x = x->idom_by;
                } else  {
                    break;
                }
            }
        }
    }
}

void DFG::make_def_use() {
    for (auto it: functions) {
        make_func_def_use(it.second);
    }
}

void DFG::make_func_def_use(Function *func) {
    for (auto *bb : func->blocks) {
        for (auto *instr : bb->instructions) {
            for (Value *v : instr->used) {
                if (dynamic_cast<LLVMInstr *>(v) && !bb->live_def.count(v)){
                    bb->live_use.insert(v);
                }
            }
            if (!instr->type->is_void_type() && !bb->live_use.count(instr)) {
                bb->live_def.insert(instr);
            }
        }
    }
}
