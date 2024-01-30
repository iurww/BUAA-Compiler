//
// Created by wwr on 2023/9/20.
//

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <unordered_map>
#include <string>
#include <vector>
#include "../util/File.h"
#include "Token.h"


class Lexer {
public:
    TokenList token_list;
    int line_cnt;
    int index;
    Token *cur_token;

    Lexer() {
        token_list = TokenList();
        line_cnt = 1;
        index = 0;
        cur_token = nullptr;
        cur_token = get_next_token();
    }

    bool is_digital(char c) {
        return c >= '0' && c <= '9';
    }

    bool is_letter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    bool is_space(char c) {
        return c == ' ' || c == '\t' || c == '\r';
    }

    char get_char() {
        return FileDealer::file_str[index++];
    }

    char peek() {
        return FileDealer::file_str[index];
    }

    void unget_char() {
        index--;
    }

    Token *get_next_token() {
        char c = get_char();
        while (c != 0) {
            std::string content = "";
            while (is_space(c)) c = get_char();
            if (c == '/') {
                if (peek() == 0) break;
                if (peek() == '/') {
                    c = get_char();
                    while ((c = get_char()) != '\n') {
                        if (c == 0) return nullptr;
                    }
                    line_cnt++;
                    c = get_char();
                    continue;
                } else if (peek() == '*') {
                    c = get_char();
                    while ((c = get_char()) != 0) {
                        if (c == '\n') line_cnt++;
                        if (c == '*') {
                            if (peek() == '/') {
                                c = get_char();
                                break;
                            } else continue;
                        }
                    }
                    c = get_char();
                    continue;
                }
                return new Token(TokenType::DIV, "/", line_cnt);
            } else if (c == '!') {
                if (peek() == '=') {
                    c = get_char();
                    return new Token(TokenType::NEQ, "!=", line_cnt);
                }
                return new Token(TokenType::NOT, "!", line_cnt);
            } else if (c == '&') {
                if (peek() == '&') {
                    c = get_char();
                    return new Token(TokenType::AND, "&&", line_cnt);
                }
                return new Token(TokenType::UNDEFINED, "&", line_cnt);
            } else if (c == '|') {
                if (peek() == '|') {
                    c = get_char();
                    return new Token(TokenType::OR, "||", line_cnt);
                }
                return new Token(TokenType::UNDEFINED, "|", line_cnt);
            } else if (c == '+') {
                return new Token(TokenType::PLUS, "+", line_cnt);
            } else if (c == '-') {
                return new Token(TokenType::MINU, "-", line_cnt);
            } else if (c == '*') {
                return new Token(TokenType::MULT, "*", line_cnt);
            } else if (c == '%') {
                return new Token(TokenType::MOD, "%", line_cnt);
            } else if (c == '<') {
                if (peek() == '=') {
                    c = get_char();
                    return new Token(TokenType::LEQ, "<=", line_cnt);
                }
                return new Token(TokenType::LSS, "<", line_cnt);
            } else if (c == '>') {
                if (peek() == '=') {
                    c = get_char();
                    return new Token(TokenType::GEQ, ">=", line_cnt);
                }
                return new Token(TokenType::GRE, ">", line_cnt);
            } else if (c == '=') {
                if (peek() == '=') {
                    c = get_char();
                    return new Token(TokenType::EQL, "==", line_cnt);
                }
                return new Token(TokenType::ASSIGN, "=", line_cnt);
            } else if (c == ';') {
                return new Token(TokenType::SEMICN, ";", line_cnt);
            } else if (c == ',') {
                return new Token(TokenType::COMMA, ",", line_cnt);
            } else if (c == '(') {
                return new Token(TokenType::LPARENT, "(", line_cnt);
            } else if (c == ')') {
                return new Token(TokenType::RPARENT, ")", line_cnt);
            } else if (c == '[') {
                return new Token(TokenType::LBRACK, "[", line_cnt);
            } else if (c == ']') {
                return new Token(TokenType::RBRACK, "]", line_cnt);
            } else if (c == '{') {
                return new Token(TokenType::LBRACE, "{", line_cnt);
            } else if (c == '}') {
                return new Token(TokenType::RBRACE, "}", line_cnt);
            } else if (is_digital(c)) {
                content += c;
                while (is_digital(peek())) {
                    c = get_char();
                    content += c;
                }
                return new Token(TokenType::INTCON, content, line_cnt);
            } else if (is_letter(c)) {
                content += c;
                while (is_letter(peek()) || is_digital(peek())) {
                    c = get_char();
                    content += c;
                }
                if (reserve_to_token.count(content) > 0)
                    return new Token(reserve_to_token[content], content, line_cnt);
                else
                    return new Token(TokenType::IDENFR, content, line_cnt);
            } else if (c == '\"') {
                content += c;
                while ((c = get_char()) != '\"') {
                    content += c;
                }
                content += c;
                return new Token(TokenType::STRCON, content, line_cnt);
            } else if (c == '\n') {
                line_cnt++;
                c = get_char();
            }
        }
        return nullptr;
    }

    void lex() {
        while (cur_token != nullptr) {
            token_list.append(cur_token);
            cur_token = get_next_token();
        }
    }

};

#endif //COMPILER_LEXER_H
