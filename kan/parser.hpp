//
// Created by kvxmmu on 7/4/20.
//

#ifndef KANLANG_PARSER_HPP
#define KANLANG_PARSER_HPP

#include <kan/lexer.hpp>


std::pair<size_t, AstTree> parse(const token_vector_t &tokens,
                                 size_t start_pos = 0, Token *until_token = nullptr,
                                 priority_t block_priority = NORMAL_PRIORITY);

#endif //KANLANG_PARSER_HPP
