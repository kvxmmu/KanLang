#include <iostream>

#include "kan/parser.hpp"
#include "kan/executor/compiler.hpp"
#include "kan/executor/executor.hpp"

int main() {
    auto code = "a = \"пизда? да\".print()+2;";
    Kan::AstTree tree;

    auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);

    Kan::token_iterator_t it(tokens);

    Kan::parse_tree(Kan::TokenTypes::SKIP, it,
            &tree);

    auto data = Kan::Executor::compile(tree);

    return 0;
}
