//
// Created by wwr on 2023/9/20.
//

#ifndef COMPILER_FILE_H
#define COMPILER_FILE_H

#include <iostream>
#include <fstream>
#include <cassert>

const std::string input_file_name = "testfile.txt";
const std::string output_file_name = "output.txt";
const std::string error_file_name = "error.txt";
const std::string llvm_file_name = "llvm_ir.txt";
const std::string mips_file_name = "mips.txt";

class Lexer;

class Parser;

class FileDealer {
public:
    static FILE *fp_input;
    static char *file_str;

    static void read_input_file() {
        fp_input = fopen(input_file_name.c_str(), "r");
        assert(fp_input != nullptr);

        fseek(fp_input, SEEK_SET, SEEK_END);
        unsigned int file_size = ftell(fp_input);
        file_str = new char[file_size + 1]();

        rewind(fp_input);
        fread(file_str, 1, file_size, fp_input);
        file_str[file_size] = 0;
    }

    static void print_lexer_result(Lexer &lexer);

    static void print_parser_result(Parser &parse);

    static void print_errors();

    static void print_llvm();

    static void print_mips();

};

#endif //COMPILER_FILE_H
