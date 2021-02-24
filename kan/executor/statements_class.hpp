//
// Created by kvxmmu on 2/16/21.
//

#ifndef KANLANG_STATEMENTS_CLASS_HPP
#define KANLANG_STATEMENTS_CLASS_HPP

#include <vector>
#include <string_view>

#include "../bytebuffer.hpp"
#include "bytecode_def.h"

namespace Kan::Statements {
    class CompileStream {
    public:
        size_t offset = 0;

        template <typename T>
        void write_integral(T integral) {
            char bin[sizeof(T)];

            int_to_bytes<T>(integral, bin);

            this->write_to_stream(bin, sizeof(T));
        }

        virtual void write_to_stream(const char *_data, size_t size) = 0;
        virtual void read_from_stream(char *dest, size_t size) = 0;

        void push_empty_command(uint8_t command_type) {
            this->write_integral<uint8_t>(command_type);
        }

        void push_sized_command(uint8_t command_type, const char *_data,
                                uint16_t size = 0) {
            this->write_integral<uint8_t>(command_type);
            this->write_integral<uint16_t>(size);
            this->write_to_stream(_data, size);
        }

        void get_attribute() {
            this->push_empty_command(GET_ATTRIBUTEB);
        }

        void get_attribute(std::string_view attr) {
            this->push_sized_command(GET_ATTRIBUTEB,
                                     const_cast<char *>(attr.data()), attr.size());
        }

        void load_id(std::string_view id) {
            this->push_sized_command(LOAD_ID, id.data(), id.size());
        }

        void push_string(std::string_view string) {
            this->push_sized_command(PUSH_STRING, string.data(),
                                     string.size());
        }

        template <typename T1>
        void push_integral(T1 integral) {
            char bin[sizeof(T1)];

            int_to_bytes<T1>(integral, bin);

            this->push_sized_command(PUSH_INTEGRAL, bin,
                                     sizeof(T1));
        }

        void plus() { this->push_empty_command(PLUSB); }
        void minus() { this->push_empty_command(MINUSB); }
        void pow() { this->push_empty_command(POWB); }
        void div() { this->push_empty_command(DIVB); }
        void mul() { this->push_empty_command(MULB); }
        void call() { this->push_empty_command(CALLB); }
        void uminus() { this->push_empty_command(UMINUS); }
        void uplus() { this->push_empty_command(UPLUS); }
    };

    class VectorCompileStream : public CompileStream {
    public:
        std::vector<unsigned char> bin_data;

        void write_to_stream(const char *_data, size_t size) override {
            this->bin_data.insert(this->bin_data.begin()+this->offset,
                                  _data, _data+size);

            this->offset += size;
        }

        void read_from_stream(char *dest, size_t size) override {
            memcpy(dest, this->bin_data.data()+size+this->offset, size);

            this->offset += size;
        }
    };
}

#endif //KANLANG_STATEMENTS_CLASS_HPP
