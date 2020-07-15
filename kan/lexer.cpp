//
// Created by kvxmmu on 7/3/20.
//

#include "lexer.hpp"

std::string crop_string(const std::string &source, size_t until) {
    size_t length = source.size();

    if (length < until) {
        return source;
    }

    return source.substr(0, until);
}

unsigned int count_lines(const std::string &string, unsigned int until_pos) {
    unsigned int lines = 0;

    for (auto &c : string) {
        if (c == '\n') {
            lines++;
        }
    }

    return lines;
}

std::string build_token_tb(string_iterator_t &iterator) {
    std::string cropped = crop_string(iterator.container.substr(iterator.pos), SHOW_CODE_LEN);
    unsigned int lines = count_lines(iterator.container, iterator.size);

    return cropped + " on line " + std::to_string(lines);
}

token_vector_t lex(const std::string &source,
                   const std::vector<lexer_function_t> &lexers) {
    token_vector_t tokens;

    string_iterator_t iterator(source);

    iterator.peek(0);

    bool is_token_found = false;

    while (!iterator.is_done()) {
        for (auto &lexer_function : lexers) {
            is_token_found = lexer_function(tokens, iterator);

            if (is_token_found) {
                break;
            }
        }

        if (is_token_found) {
            is_token_found = false;
        } else {
            Traceback(PARSE_ERROR, "Unknown token "+build_token_tb(iterator)).throw_traceback();
        }
    }

    return tokens;
}

bool full_equals(string_iterator_t &iterator, const std::string &op) {
    size_t pos = 0;
    size_t size = op.size();

    for (; pos < size; pos++) {
        auto current_char = iterator.peek(pos);

        if (current_char != op[pos]) {
            return false;
        }
    }

    return pos >= op.size();
}

////////////////

bool is_operator(token_vector_t &tokens, string_iterator_t &iterator) {
    const std::vector<Operator> operators = {
            Operator {".", DOT, HALF_PRIORITY},
            Operator {"+", PLUS},
            Operator {"-", MINUS},
            Operator {"**", POW, FULL_PRIORITY},
            Operator {"%", MOD, HALF_PRIORITY},
            Operator {"*", MUL, HALF_PRIORITY},
            Operator {"==", EQUALS},
            Operator {"!=", NOT_EQUALS},
            Operator {"!", NOT},
            Operator {">=", GREATER_OR_EQUALS},
            Operator {"<=", LESSER_OR_EQUALS},
            Operator {"/", DIV},
            Operator {"%", MOD}
    };

    for (auto &op : operators) {
        if (full_equals(iterator, op.text)) {
            tokens.emplace_back(op.text, OPERATOR, op.priority,
                    op.subtype);

            iterator.next(op.text.size());

            return true;
        }
    }

    return false;
}

bool is_number(token_vector_t &tokens, string_iterator_t &iterator) {
    std::string number;

    if (!isdigit(iterator.peek(0))) {
        return false;
    }

    while (!iterator.is_done(number.size())) {
        auto chr = iterator.peek(number.size());

        if (!isdigit(chr)) {
            break;
        }

        number += chr;

    }

    tokens.emplace_back(number, NUMBER);
    iterator.next(number.size());

    return true;
}

bool is_breakline(token_vector_t &tokens, string_iterator_t &iterator) {
    auto keys = {
            ' ', '\t', '\b', '\r'
    };

    for (auto &key : keys) {
        if (key == iterator.peek(0)) {
            iterator.next();

            return true;
        }
    }

    return false;
}

bool is_id(token_vector_t &tokens, string_iterator_t &iterator) {
    std::string id;
    size_t pos = 0;

    char first_char = iterator.peek(pos);

    if (isdigit(first_char)) {
        return false;
    }

    while (isalpha(first_char) || isdigit(first_char)) {
        id += first_char;

        first_char = iterator.peek(++pos);
    }

    if (id.empty()) {
        return false;
    }

    iterator.next(pos);

    tokens.emplace_back(id, ID);

    return true;
}

char create_hex_chars(char first, char second) {
    char out = 0;
    std::stringstream stream;

    stream << std::hex << std::string({first, second, 0});

    stream >> out;

    return out;
}

std::pair<char, size_t> convert_char(size_t pos, string_iterator_t &iterator) {
    char next_char = iterator.peek(pos);

    char chr = 0;
    size_t size = 0;

    switch (next_char) {
        case 'n':
            chr = '\n';
            break;

        case 'r':
            chr = '\r';
            break;

        case 'x':
            chr = create_hex_chars(iterator.peek(pos+1), iterator.peek(pos+2));
            size = 2;
            break;

        case 't':
            chr = '\t';
            break;

        case 'b':
            chr = '\b';
            break;

        case '"':
            chr = '"';
            break;

        default:
            chr = next_char;
    }

    return std::make_pair(chr, size);
}

bool is_string(token_vector_t &tokens, string_iterator_t &iterator) {
    size_t pos = 0;

    char first_char = iterator.peek(pos);

    std::string str;

    if (first_char != '"') {
        return false;
    }

    while (true) {
        first_char = iterator.peek(++pos);

        if (first_char == '\\') {
            auto chr_pair = convert_char(pos+1, iterator);

            str += chr_pair.first;
            pos += chr_pair.second+1;
            continue;
        } else if (first_char == '"') {
            break;
        }

        str += first_char;
    }

    tokens.emplace_back(str, STRING);
    iterator.next(pos+1);

    return true;
}

bool is_brackets(token_vector_t &tokens, string_iterator_t &iterator) {
    char bracket_char = iterator.peek(0);
    token_t type;

    switch (bracket_char) {
        case '(':
            type = OPEN_BRACKET;
            break;

        case ')':
            type = CLOSE_BRACKET;
            break;

        case '{':
            type = OPEN_BRACE;
            break;

        case '}':
            type = CLOSE_BRACE;
            break;

        case '[':
            type = OPEN_QBRACKET;
            break;

        case ']':
            type = CLOSE_QBRACKET;
            break;

        default:
            return false;
    }

    iterator.next();
    tokens.emplace_back(std::string(&bracket_char, 1), type);

    return true;
}

bool is_arrow(token_vector_t &tokens, string_iterator_t &iterator) {
    char first = iterator.peek(0);
    char second = iterator.peek(1);

    bool done = false;
    if (first == '-' && (second == '>' || second == '<')) {
        done = true;

    }

    return done;
}

///////////////
