#include <iostream>

#include "kan/parser.hpp"

int main() {
    auto code = "a = 1+2;\n"
                "b = 2+2;";

    auto tokens = Kan::parse_tokens(code, Kan::Tokenizers::default_all_incl);

    for (auto &token : tokens) {
        std::cout << token.token << std::endl;
    }

    return 0;
}
