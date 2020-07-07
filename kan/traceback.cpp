//
// Created by kvxmmu on 7/3/20.
//

#include "traceback.hpp"

void Traceback::throw_traceback() const {
    std::cerr << TRACEBACK_PREFIX << this->trace_title << ":\n\t";

    std::cerr << "On line " << this->line << ':' << this->char_pos << ": " << this->text << std::endl;

    exit(TRACEBACK_EXIT);
}
