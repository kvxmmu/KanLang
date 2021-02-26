#include <iostream>

#include "kan/parser.hpp"
#include "kan/executor/compiler.hpp"

#include "kan/vm/vm.hpp"

int main() {
    auto code = "a = \"pohui\".print();";
    Kan::AstTree tree;

    auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);

    Kan::token_iterator_t it(tokens);

    Kan::parse_tree(Kan::TokenTypes::SKIP, it,
            &tree);

    auto data = reinterpret_cast<Kan::Statements::VectorCompileStream *>(Kan::Executor::compile(tree));
    data->seek(0);

    Kan::VM::kanvm_execute(data);

    delete data;

    return 0;
}
