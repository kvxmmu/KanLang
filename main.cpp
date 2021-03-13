#include <iostream>

#include "kan/easy.hpp"

int main() {
    std::string code;

    while (true) {
        std::cout << "Введите код(.q чтобы выйти): ";
        std::getline(std::cin, code);

        if (code == ".q") {
            break;
        }

        Kan::run(code);
    }


    return 0;
}
