//
// Created by kvxmmu on 7/3/20.
//

#ifndef KANLANG_TRACEBACK_HPP
#define KANLANG_TRACEBACK_HPP

#include <kan/types.hpp>
#include <iostream>

#ifndef TRACEBACK_PREFIX
# define TRACEBACK_PREFIX "<Kan>Exception::"
#endif

#define TRACEBACK_EXIT 0x2u

class Traceback {
public:
    std::string trace_title;
    std::string text;

    unsigned int line;
    unsigned int char_pos;

    explicit Traceback(std::string _trace_title,
            std::string _text,
            unsigned int _line = 1,
            unsigned int _char_pos = 0) : trace_title(std::move(_trace_title)),
                line(_line), char_pos(_char_pos), text(std::move(_text)) {

    }

    void throw_traceback() const;

};


#endif //KANLANG_TRACEBACK_HPP
