//
// Created by kvxmmu on 7/3/20.
//

#ifndef KANLANG_LEXER_HPP
#define KANLANG_LEXER_HPP

#include <kan/types.hpp>
#include <kan/traceback.hpp>

#include <functional>
#include <sstream>

typedef std::vector<Token> token_vector_t;
typedef PeekableIterator<token_vector_t> token_iterator_t;
typedef ConstPeekableIterator<std::string> string_iterator_t;

typedef std::function<bool(token_vector_t &, string_iterator_t &)> lexer_function_t;

std::string crop_string(const std::string &source, size_t until);
std::string build_token_tb(string_iterator_t &iterator);
unsigned int count_lines(const std::string &string, unsigned int until_pos);

////////////////

bool is_operator(token_vector_t &tokens, string_iterator_t &iterator);
bool is_number(token_vector_t &tokens, string_iterator_t &iterator);
bool is_id(token_vector_t &tokens, string_iterator_t &iterator);
bool is_breakline(token_vector_t &tokens, string_iterator_t &iterator);
bool is_string(token_vector_t &tokens, string_iterator_t &iterator);
bool is_brackets(token_vector_t &tokens, string_iterator_t &iterator);
bool is_arrow(token_vector_t &tokens, string_iterator_t &iterator);

///////////////

token_vector_t lex(const std::string &source,
                   const std::vector<lexer_function_t> &lexers);

#endif //KANLANG_LEXER_HPP
