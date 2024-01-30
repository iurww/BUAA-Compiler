#include "../include/util/File.h"
#include "../include/frontend/Lexer.h"
#include "../include/frontend/Parser.h"
#include "../include/frontend/Visitor.h"
#include "../include/util/config.h"
#include "../include/backend/CodeGen.h"


int main() {

    FileDealer::read_input_file();

    Lexer lexer = Lexer();
    lexer.lex();
#ifdef OUTPUT_LEXER_RESULT
    FileDealer::print_lexer_result(lexer);
#endif

    Parser parser = Parser(lexer.token_list);
    AST::CompUnit *root = parser.parseCompUnit();
#ifdef OUTPUT_PARSER_RESULT
    FileDealer::print_parser_result(parser);
#endif

    Visitor visitor = Visitor();
    visitor.visitCompUnit(root);
#ifdef OUTPUT_ERRORS
    FileDealer::print_errors();
#endif
#ifdef OUTPUT_LLVM
    FileDealer::print_llvm();
#endif

    CodeGen::CODE_GEN->gen_mips();
#ifdef OUTPUT_MIPS
    FileDealer::print_mips();
#endif

    return 0;
}
