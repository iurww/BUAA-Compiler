//
// Created by wwr on 2023/12/12.
//

#ifndef COMPILER_REGALLOCATOR_H
#define COMPILER_REGALLOCATOR_H

#include <unordered_map>
#include <string>

class Function;

class RegAllocator {
public:
    std::unordered_map<std::string, Function *> &functions;

    RegAllocator(std::unordered_map<std::string, Function *> &functions) : functions(functions) {};

};


#endif //COMPILER_REGALLOCATOR_H
