//
// Created by kvxmmu on 2/12/21.
//

#include "parser.hpp"

/// AstTree

std::shared_ptr<Kan::AstObject> Kan::AstTree::add_tree(Kan::TreeType _type) {
    auto tree = std::make_shared<AstObject>(_type);

    this->objects.push_back(tree);

    return tree;
}

std::shared_ptr<Kan::AstObject> Kan::AstTree::add_token(const Kan::Token &_token) {
    auto object = std::make_shared<Kan::AstObject>(_token);

    this->objects.push_back(object);

    return object;
}

/// Tokenizer

std::vector<Kan::Token> Kan::parse_tokens(std::string_view code,
                                     const std::vector<Kan::token_parser_t> &parsers) {
    std::vector<Kan::Token> tokens;
    Kan::Iterator<std::string_view> it(code, NULLCHR);

    bool success = false;

    while (!it.is_done(0)) {
        for (auto &parser : parsers) {
            success = parser(it, tokens);

            if (success) {
                break;
            }
        }

        if (!success) {
            std::cout << "Pohui " << it.pos << std::endl;

            return {};
        } else {
            success = false;
        }
    }

    return tokens;
}

