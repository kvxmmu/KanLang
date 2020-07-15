//
// Created by kvxmmu on 7/4/20.
//

#include "parser.hpp"

std::pair<size_t, AstTree> parse(const token_vector_t &tokens,
                                 size_t start_pos, Token *until_token,
                                 priority_t block_priority) {
    AstTree tree;
    ConstPeekableIterator<token_vector_t> iterator(tokens);
    iterator.pos = start_pos;

    Token close_bracket(")", CLOSE_BRACKET, BRACKET_PRIORITY);
    Token close_qbracket("]", CLOSE_QBRACKET, QBRACKET_PRIORITY);
    Token close_brace("}", CLOSE_BRACE, BRACES_PRIORITY);

    while (!iterator.is_done()) {
        auto &current_token = iterator.peek(0);

        if (until_token != nullptr && current_token.type == until_token->type) {
            iterator.next();
            break;
        }

        if (current_token.type == OPEN_BRACKET || current_token.type == OPEN_BRACE
            || current_token.type == OPEN_QBRACKET) {
            Token *use_object = nullptr;

            switch (current_token.type) {
                case OPEN_BRACE:
                    use_object = &close_brace;
                    break;

                case OPEN_BRACKET:
                    use_object = &close_bracket;
                    break;

                case OPEN_QBRACKET:
                    use_object = &close_qbracket;
                    break;

                default:
                    throw std::runtime_error("Unknown object type(parsing)");
            }

            auto new_tree_pair = parse(tokens, iterator.pos+1, use_object,
                                       use_object->priority);
            auto tree_copy = new AstTree;

            *tree_copy = new_tree_pair.second;
            tree_copy->tree_type = current_token.type;
            tree_copy->need_free = true;
            tree_copy->priority = use_object->priority;

            tree.emplace_back(tree_copy);

            iterator.next(new_tree_pair.first - iterator.pos - 1);
        } else {
            tree.emplace_back(current_token);
        }

        iterator.next();
    }

    return std::make_pair(iterator.pos, tree);
}
