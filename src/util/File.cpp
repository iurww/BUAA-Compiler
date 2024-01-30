//
// Created by wwr on 2023/9/20.
//

#include <cstdio>
#include <algorithm>
#include "../../include/util/File.h"
#include "../../include/frontend/Lexer.h"
#include "../../include/frontend/Parser.h"
#include "../../include/frontend/Error.h"
#include "../../include/mir/IRModule.h"
#include "../../include/backend/CodeGen.h"

FILE *FileDealer::fp_input;
char *FileDealer::file_str;

void FileDealer::print_lexer_result(Lexer &lexer) {
    std::ofstream f;
    f.open(output_file_name);
    f << lexer.token_list;
}

void FileDealer::print_parser_result(Parser &parser) {
    std::ofstream f;
    f.open(output_file_name);
    for (auto s: parser.result) {
//           std::cout << s << std::endl;
        f << s << std::endl;
    }
}

void FileDealer::print_errors() {
    std::ofstream f;
    std::vector<std::pair<int, char>> &errors = ErrorHandler::ERROR_HANDLER->errors;
    std::sort(errors.begin(), errors.end());
    f.open(error_file_name);
    for (const auto p: ErrorHandler::ERROR_HANDLER->errors) {
        f << p.first << " " << p.second << std::endl;
    }
    if (errors.size()) {
        std::cout << "find errors...." << std::endl;
        exit(0);
    }
}

void FileDealer::print_llvm() {
    std::ofstream f;
    f.open(llvm_file_name);
    f << *(IRModule::IR_MODULE);
}

void FileDealer::print_mips() {
    std::ofstream f;
    f.open(mips_file_name);
    f << *(CodeGen::CODE_GEN);
}