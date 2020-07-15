#include <iostream>

#include <kan/compiler.hpp>
#include <kan/lexer.hpp>
#include <kan/parser.hpp>

#define NOPUSH 0xff

#include <deque>

template <typename T>
T calculate(AstTree &tree);

template <typename T>
T mix_operator(Token &op, const Token &number_tkn) {
    T out = std::stoll(number_tkn.value);

    switch (op.subtype) {
        case MINUS:
            return -out;

        case PLUS:
            return +out;

        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

template <typename T>
T mix_operator(Token &op, const T &out) {
    switch (op.subtype) {
        case MINUS:
            return -out;

        case PLUS:
            return +out;

        default:
            throw std::runtime_error("Unknown unary operator");
    }
}

template <typename T>
std::pair<size_t, ast_object_type_t> find_highest_priority(AstTree &tree) {
    size_t max_pos = std::string::npos;
    size_t max_priority = std::string::npos;

    ast_object_type_t block_type = NO_TYPE;

    for (size_t pos = 0; pos < tree.size();) {
        AstObject &first = tree.at(pos);

        if (first.type == TREE) {
            max_pos = pos;

            break;
        }

        if (pos+1 >= tree.size()) {
            break;
        }

        if (first.type == TOKEN && first.token.type == OPERATOR) {
            AstObject &number = tree.at(pos+1);

            if (number.type == TREE) {
                max_pos = pos+1;
                block_type = number.type;

                break;
            } else {
                auto result = mix_operator<T>(first.token, number.token);

                tree.replace(pos, 2, Token(std::to_string(result), NUMBER));

                continue;
            }
        }

        Token &op = tree.at(pos+1).token;

        AstObject &right_object = tree.at(pos+2);

        if (right_object.type == TREE) {
            max_pos = pos+2;

            break;
        }

        if (max_priority == std::string::npos || max_priority < op.priority) {
            max_pos = pos;
            max_priority = op.priority;
        }

        pos += 2;
    }

    return std::make_pair(max_pos, block_type);
}

template <typename T>
T calculate_child(const Token &left_tkn, const Token &right_tkn,
        const Token &op) {
    T left = std::stoll(left_tkn.value);
    T right = std::stoll(right_tkn.value);

    switch (op.subtype) {
        case MINUS:
            return left - right;

        case PLUS:
            return left + right;

        case MUL:
            return left * right;

        case DIV:
            return left / right;

        case MOD:
            return left % right;

        default:
            throw std::runtime_error("Unknown pizdec");
    }
}

template <typename T>
T calculate(AstTree &tree) {
    while (true) {
        auto pos_pair = find_highest_priority<T>(tree);

        size_t pos = pos_pair.first;
        ast_object_type_t block_type = pos_pair.second;

        if (pos == std::string::npos) {
            break;
        }

        AstObject &left_ast = tree.at(pos);

        if (left_ast.type == TREE) {
            auto result = calculate<T>(*left_ast.tree);

            tree.replace(pos, 1, Token(std::to_string(result), NUMBER));

            continue;
        }

        Token &left = left_ast.token;
        Token &op = tree.at(pos+1).token;
        Token &right = tree.at(pos+2).token;

        if (right.type == OPERATOR) {
            AstObject &shifted = tree.at(pos+3);

            if (shifted.type == TREE) {
                auto result = calculate<T>(*shifted.tree);
                result = mix_operator<T>(right, result);

                tree.replace(pos+2, 2, Token(std::to_string(result), NUMBER));

                continue;
            }

            auto result = calculate<T>(*shifted.tree);

            tree.replace(pos+2, 2, Token(std::to_string(result), NUMBER));
        }

        auto result = calculate_child<T>(left, right, op);

        tree.replace(pos, 3, Token(std::to_string(result), NUMBER));
    }


    return std::stoll(tree[0].token.value);
}

int main() {
    std::vector<lexer_function_t> lexers = {
        is_number,
        is_operator,
        is_breakline,
        is_brackets
    };

    std::string line;

    while (true) {
        std::cout << "calc> ";
        std::getline(std::cin, line);

        if (line == "quit") {
            break;
        }

        auto lexed = lex(line, lexers);
        auto parsed = parse(lexed).second;

        std::cout << calculate<long>(parsed) << std::endl;

        parsed.free();
    }

    return 0;
}
