#include <iostream>

#include <kan/compiler.hpp>
#include <kan/lexer.hpp>

#define NOPUSH 0xff

#include <deque>

size_t find_highest_priority(const token_vector_t &tokens) {
    size_t max_priority = std::string::npos;
    size_t max_pos = 0;

    for (size_t pos = 0; pos < tokens.size(); pos++) {
        const Token &token = tokens[pos];

        if (token.type != OPERATOR) {
            continue;
        }

        if (max_priority == std::string::npos) {
            max_pos = pos;
            max_priority = token.priority;

            continue;
        }

        if (max_priority < token.priority) {
            max_pos = pos;
            max_priority = token.priority;
        }
    }

    if (max_priority == std::string::npos) {
        return max_priority;
    }

    return max_pos-1;
}

void determine_operator(KanIR &ir, Token &op) {
    switch (op.subtype) {
        case PLUS:
            ir.addq();
            break;

        case MUL:
            ir.mulq();
            break;

        case DIV:
            ir.divq();
            break;

        case MINUS:
            ir.subq();
            break;
    }
}

KanIR compile_expr(token_vector_t tokens) {
    KanIR ir;

    while (tokens.size() > 1) {
        size_t pos = find_highest_priority(tokens);

        if (pos == std::string::npos) {
            throw std::runtime_error("Всем похуй.");
        }

        Token &left = tokens[pos];
        Token &op = tokens[pos+1];
        Token &right = tokens[pos+2];

        if (left.has_flag(NOPUSH) && right.has_flag(NOPUSH)) {
            break;
        } else if (left.has_flag(NOPUSH)) {
            ir.push("float32", right.value);
            determine_operator(ir, op);

        } else if (right.has_flag(NOPUSH)) {
            ir.push("float32", left.value);
            determine_operator(ir, op);

        } else {
            ir.push("float32", left.value);
            ir.push("float32", right.value);
            determine_operator(ir, op);
        }



        tokens.erase(tokens.begin()+pos, tokens.begin()+pos+3);

        Token new_token("xxx", NO_TYPE);
        new_token.flags = NOPUSH;

        tokens.insert(tokens.begin()+pos, new_token);

    }

    return ir;
}

template <typename T>
T eval_op(T left, T right,
        const std::string &op) {
    if (op == ADD_COMMAND) {
        return left + right;
    } else if (op == DIV_COMMAND) {
        return left / right;
    } else if (op == SUB_COMMAND) {
        return left - right;
    } else if (op == MUL_COMMAND) {
        return left * right;
    }

    throw std::runtime_error("Unknown operator");
}

template <typename T>
T eval(const KanIR &ir) {
    std::deque<T> stack;

    for (auto &command : ir.command_layer) {
        size_t endpos = command.find(' ');

        if (endpos != std::string::npos) { // two argument
            std::string args = command.substr(endpos+1);
            std::string needle = args.substr(args.find(' ')+1);

            stack.push_back(std::stoi(needle));

            continue;
        }

        T left = stack.front();
        stack.pop_front();

        T right = stack.front();
        stack.pop_front();

        stack.push_back(eval_op(left, right, command));
    }

    return stack.front();
}

int main() {
    std::vector<lexer_function_t> lexers = {
        is_number,
        is_operator,
        is_breakline
    };

    std::string line;

    while (true) {
        std::cout << "KanCalc> ";
        std::getline(std::cin, line);

        if (line == "quit") {
            break;
        } else if (line.empty()) {
            std::cout << "Enter quit to exit from calc" << std::endl;

            continue;
        }

        auto tokens = lex(line, lexers);
        auto ir = compile_expr(tokens);

        std::cout << "IR: " << ir.compile() << std::endl;
        std::cout << "Eval: " << eval<float>(ir) << std::endl;
    }


    return 0;
}
