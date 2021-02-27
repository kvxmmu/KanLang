//
// Created by kvxmmu on 2/13/21.
//

#ifndef KANLANG_COMPILER_HPP
#define KANLANG_COMPILER_HPP

#include <unordered_map>
#include <variant>
#include <type_traits>

#include "../parser.hpp"
#include "stmt_parser.hpp"

#include "../bytebuffer.hpp"

#include "statements_class.hpp"
#include "../types.h"

#define IF_NAME "if"
#define ELSE_NAME "else"
#define FUNC_NAME "func"
#define CLASS_NAME "class"
#define IMPORT_NAME "import"

#include "bytecode_def.h"

#define LOW_PRIORITY 1u
#define MIDDLE_PRIORITY 2u
#define HIGH_PRIORITY 3u
#define VHIGH_PRIORITY 4u
#define TOP_PRIORITY 5u
#define VTOP_PRIORITY 6u

#define CHECK_AND_SET_PRIORITY(priority, pos, _type) if (max_priority < priority) {\
    max_priority = priority; \
    max_pos = pos;\
    max_type = _type;\
}

enum StatementType {
    IF,
    VARIABLE_ASSIGN,
    FUNCTION,
    CLASS,
    IMPORT,
    ELSE,
    ELSEIF,

    EXPRESSION
};

using stmt_cond = Kan::StatementCondition;
using stmt = Kan::StatementParser<StatementType>;

namespace Kan::Executor {
    enum ResponseType {
        BRACKETS,
        BINARY_OP,
        UNARY_OP,
        CALL,
        DONE,
        ERROR
    };

    struct SyntaxError : public std::runtime_error {
        SyntaxError(size_t n = 0) : std::runtime_error("Invalid syntax "+std::to_string(n)) {

        }
    };

    typedef std::pair<ResponseType, size_t> operation_pair_t;
    typedef uint8_t priority_t;

    operation_pair_t get_operation(ast_iterator_t &it);
    priority_t get_operator_priority(const Token &token);
    priority_t get_priority(const ast_objects_t::value_type &object);
    bool is_operator(const ast_objects_t::value_type &object);

    std::vector<ast_objects_t> split_by_token(const ast_objects_t &objects,
                                              TokenTypes token_type);

    uint32_t compile_expression(Kan::Statements::CompileStream *stream,
                                ast_objects_t &objects, bool is_arguments=false);
    bool is_integral_type(const Token &token);
    void compile_literal(const Token &literal, Kan::Statements::CompileStream *stream,
            bool is_attribute = false);
    void replace_object_with_empty_token(ast_objects_t &objects, size_t index,
            size_t count);
    bool is_unary_operator(ast_objects_t::value_type &object);
    void compile_brackets(Kan::Statements::CompileStream *stream,
                                         const ast_objects_t::value_type &obj, size_t pos,
                                         ast_objects_t &objects);
    size_t compile_function_signature(Kan::Statements::CompileStream *stream,
            const ast_objects_t &objects);

    void compile(Kan::AstTree &tree, Kan::Statements::CompileStream *stream,
            bool create_scope = true, bool is_function = false);
}


#endif //KANLANG_COMPILER_HPP
