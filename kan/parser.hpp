//
// Created by kvxmmu on 2/12/21.
//

#ifndef KANLANG_PARSER_HPP
#define KANLANG_PARSER_HPP

#include <string>
#include <utility>
#include <vector>
#include <string_view>

#include <functional>

#include <memory>

#include "iterator.hpp"
#include <iostream>

#define TREEPTR 1u
#define NULLCHR '\x00'
#define ONE_CHAR_PARSER_DEF(chr, name, _ttype) bool name(string_iterator_t &it, tokens_t &tokens) {\
    try { \
        return one_char_parser(chr, _ttype)(it, tokens); \
    } catch (std::runtime_error &e) { \
        return false; \
    } \
}
#define TWO_CHARS_PARSER_DEF(fchr, schr,\
                             name, _ttype) bool name(string_iterator_t &it, tokens_t &tokens) {\
    try { \
        return two_chars_parser(fchr, schr, _ttype)(it, tokens); \
    } catch (std::runtime_error &e) { \
        return false; \
    } \
}

namespace Kan {
    enum TokenTypes {
        // Logical operators
        EQUAL,
        EQUALEQUAL,
        GREATER_OR_EQUAL,
        LESSER_OR_EQUAL,
        GREATER,
        LESSER,
        NOT_EQUAL,

        // Brackets
        OPEN_BRACKET,
        CLOSE_BRACKET,

        // Braces
        OPEN_BRACE,
        CLOSE_BRACE,


        // Arithmetic operators
        PLUS,
        MINUS,
        MUL,
        DIV,
        POW,

        // Syntax unit
        SEMICOLON,
        COLON,
        COMMA,
        DOT,

        // literals
        STRING,
        CHAR,
        INTEGER,
        FLOAT,
        ID,

        // Skip literal
        SKIP

    };

    enum AstType {
        TOKEN,
        TREE,

        ALTPTR
    };

    enum TreeType {
        MASTER,

        BRACKETS,
        BRACES,
    };

    class Token {
    public:
        std::string token;
        TokenTypes token_types = TokenTypes::SKIP;

        Token() = default;

        Token(std::string _token, TokenTypes _token_type) : token(std::move(_token)),
                                                            token_types(_token_type) {

        }

        template <typename T>
        T icast() {
            return std::stoll(this->token);
        }
    };

    class AstTree;

    class AstObject {
    public:
        std::shared_ptr<AstTree> tree = nullptr;
        Token token;

        void *altptr = nullptr;
        std::function<void(void *)> free_func = nullptr;

        AstType type = AstType::TREE;

        AstObject(void *) : type(AstType::ALTPTR) {  };

        AstObject(TreeType _ttype = TreeType::MASTER) : tree(std::make_shared<AstTree>(_ttype)) {

        }

        bool is_empty_token() {
            return this->token.token_types == TokenTypes::SKIP;
        }

        AstObject(void *_ptr,
                std::function<void(void *)> _free_func) : type(AstType::ALTPTR), altptr(_ptr),
                                                          free_func(_free_func) {

        }

        explicit AstObject(Token _token) : token(std::move(_token)), type(AstType::TOKEN) {

        }

        [[nodiscard]] bool is_tree(const TreeType _type) const;
        [[nodiscard]] bool is_tree() const;

        [[nodiscard]] AstTree &get_tree() const {
            return *this->tree;
        }

        [[nodiscard]] constexpr bool check_token_type(TokenTypes _type) const {
            return this->type == AstType::TOKEN && this->token.token_types == _type;
        }

        [[nodiscard]] const Token &get_token() const {
            return this->token;
        }
    };

    class AstTree {
    public:
        std::vector<std::shared_ptr<AstObject>> objects;

        TreeType type = TreeType::MASTER;

        explicit AstTree(TreeType _type = TreeType::MASTER) : type(_type) {
            this->objects.reserve(5u);
        }

        std::shared_ptr<AstObject> add_tree(TreeType _type);
        std::shared_ptr<AstObject> add_token(const Token &_token);
    };

    typedef std::vector<Token> tokens_t;
    typedef Iterator<std::string_view> string_iterator_t;
    typedef Iterator<tokens_t> token_iterator_t;

    typedef std::function<bool(string_iterator_t &, tokens_t &)> token_parser_t;

    static token_parser_t one_char_parser(char chr, TokenTypes type, bool push = true) {
        return [chr, type,
                push](string_iterator_t &it, tokens_t &tokens) {
            if (it.peek() == chr) {
                it.next();

                if (push) {
                    tokens.push_back(Token({chr, 0},
                                           type));
                }

                return true;
            }

            return false;
        };
    }

    static token_parser_t two_chars_parser(char _first, char _second,
            TokenTypes type, bool push = true) {
        return [_first, _second,
                type, push](string_iterator_t &it, tokens_t &tokens) {
            auto first = it.peek();
            auto second = it.peek(1);

            bool result = _first == first && _second == second;

            if (result) {
                if (push) {
                    tokens.push_back(Token({first, second, 0},
                                           type));
                }

                it.next(2);
            }

            return result;
        };
    }

    namespace Tokenizers {
        /// Only inside this namespace i'll use TokenTypes without prefix

        namespace {
            ONE_CHAR_PARSER_DEF('(', open_bracket, OPEN_BRACKET)
            ONE_CHAR_PARSER_DEF(')', close_bracket, CLOSE_BRACKET)
            ONE_CHAR_PARSER_DEF('{', open_brace, OPEN_BRACE)
            ONE_CHAR_PARSER_DEF('}', close_brace, CLOSE_BRACE)

            ONE_CHAR_PARSER_DEF('=', equal, EQUAL)

            ONE_CHAR_PARSER_DEF('+', plus, PLUS)
            ONE_CHAR_PARSER_DEF('-', minus, MINUS)
            ONE_CHAR_PARSER_DEF('*', mul, MUL)
            ONE_CHAR_PARSER_DEF('/', div, DIV)
            TWO_CHARS_PARSER_DEF('*', '*', power, POW)

            TWO_CHARS_PARSER_DEF('=', '=', equalequal, EQUALEQUAL)
            TWO_CHARS_PARSER_DEF('>', '=', greater_or_equal, GREATER_OR_EQUAL)
            TWO_CHARS_PARSER_DEF('<', '=', lesser_or_equal, LESSER_OR_EQUAL)
            TWO_CHARS_PARSER_DEF('!', '=', not_equal, NOT_EQUAL)

            ONE_CHAR_PARSER_DEF(';', semicolon, SEMICOLON)
            ONE_CHAR_PARSER_DEF(',', comma, COMMA)
            ONE_CHAR_PARSER_DEF(':', colon, COLON)

            ONE_CHAR_PARSER_DEF('.', dot, DOT)

            bool parse_skip_char(string_iterator_t &it, tokens_t &) {
                char chr = it.peek();

                bool result = chr == '\n' || chr == '\t' || chr == ' ';

                if (result) {
                    it.next();
                }

                return result;
            }

            bool parse_number(string_iterator_t &it, tokens_t &tokens) {
                std::string number;
                size_t offset = 0;

                char chr;
                bool is_float = false;

                while (true) {
                    try {
                        chr = it.peek(offset);
                    } catch (std::runtime_error &e) {
                        break;
                    }

                    if (!isdigit(chr) &&
                        chr != '.') {
                        break;
                    }

                    if (number.empty() && chr == '.') {
                        break;
                    }

                    if (chr == '.') {
                        is_float = true;
                    }

                    number += chr;
                    offset++;
                }

                if (number.empty() || !isdigit(number.back())) {
                    return false;
                }

                it.next(offset);

                tokens.push_back(Token(number, is_float ? FLOAT : INTEGER));

                return true;
            }

            bool parse_string(string_iterator_t &it, tokens_t &tokens) {
                std::string str;
                char chr;
                bool string_ended = false;

                size_t offset = 0;

                if (it.peek() != '"') {
                    return false;
                }

                offset++;

                while (true) {
                    try {
                        chr = it.peek(offset);
                    } catch (std::runtime_error &e) {
                        break;
                    }

                    if (chr == '"' || chr == NULLCHR) {
                        string_ended = true;

                        break;
                    }

                    if (chr == '\\') {
                        char next_chr = it.peek(offset+1u);

                        switch (next_chr) {
                            case 'n':
                                chr = '\n';

                                break;

                            case 't':
                                chr = '\t';

                                break;

                            case '"':
                                chr = '"';

                                break;

                            default:
                                return false;
                        }

                        offset++;
                    }

                    str += chr;
                    offset++;
                }

                bool result = string_ended;

                if (result) {
                    it.next(offset+1u);

                    tokens.push_back(Token(str, STRING));
                }

                return result;
            }

            bool parse_id(string_iterator_t &it, tokens_t &tokens) {
                std::string id;
                static std::string_view allowed_chars = "_qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789";

                size_t offset = 0;
                char chr;

                while (allowed_chars.find(it.peek(offset)) != std::string_view::npos) {
                    try {
                       chr = it.peek(offset++);
                    } catch (std::runtime_error &e) {
                        break;
                    }

                    id += chr;
                }

                bool result = !id.empty();

                if (result) {
                    tokens.push_back(Token(id, ID));

                    it.next(offset);
                }

                return result;
            }
        }

        inline static std::vector<token_parser_t> default_all_incl = {
            open_bracket,
            close_bracket,

            open_brace,
            close_brace,

            equalequal,
            greater_or_equal,
            lesser_or_equal,
            not_equal,
            equal,

            plus,
            minus,
            mul,
            div,

            parse_number,
            parse_string,
            parse_skip_char,
            parse_id,

            colon,
            semicolon,
            comma,
            dot
        };


    }

    std::vector<Token> parse_tokens(std::string_view code,
                                    const std::vector<token_parser_t> &parsers);
    void parse_tree(TokenTypes parse_until,
                    token_iterator_t &it, AstTree *tree);
}


#endif //KANLANG_PARSER_HPP
