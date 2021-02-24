//
// Created by kvxmmu on 2/18/21.
//

#ifndef KANLANG_EXECUTOR_HPP
#define KANLANG_EXECUTOR_HPP

#include <vector>
#include <cinttypes>
#include <deque>

#include "types/types.hpp"
#include "types/std/types.hpp"
#include "bytecode_def.h"

#include "../bytebuffer.hpp"

namespace Kan::Executor {
    int execute(const char *data,
                size_t data_size);
}


#endif //KANLANG_EXECUTOR_HPP
