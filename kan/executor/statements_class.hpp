//
// Created by kvxmmu on 2/16/21.
//

#ifndef KANLANG_STATEMENTS_CLASS_HPP
#define KANLANG_STATEMENTS_CLASS_HPP

#include <vector>
#include <string_view>
#include <string>

#include "../bytebuffer.hpp"
#include "bytecode_def.h"

#include <type_traits>

namespace Kan::Statements {
    typedef std::string_view name_t;

    class CompileStream {
    public:
        size_t offset = 0;

        template <typename T>
        void write_integral(T integral) {
            char bin[sizeof(T)];

            memset(bin, 0, sizeof(T));

            int_to_bytes<T>(integral, bin);

            this->write_to_stream(bin, sizeof(T));
        }

        template <typename T>
        T read_integral() {
            char data[sizeof(T)];

            this->read_from_stream(data, sizeof(T));

            return bytes_to_int<T>(data);
        }

        virtual void write_to_stream(const char *_data, size_t size) = 0;
        virtual void read_from_stream(char *dest, size_t size) = 0;
        virtual size_t size() { return 0; }

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
            this->push_sized_command(GET_ATTRIBUTESB,
                                     const_cast<char *>(attr.data()), attr.size());
        }

        void load_id(std::string_view id) {
            this->push_sized_command(LOAD_ID, id.data(), id.size());
        }

        std::string get_string(size_t len) {
            char *buffer = new char[len];
            std::string str;

            this->read_from_stream(buffer, len);
            str = std::string(buffer, len);

            delete[] buffer;

            return str;
        }

        std::string get_string_with_size() {
            auto length = this->read_integral<uint16_t>();

            return this->get_string(length);
        }

        void push_string(std::string_view string) {
            this->push_sized_command(PUSH_STRING, string.data(),
                                     string.size());
        }

        void set_name(name_t name) {
            this->push_sized_command(SET_NAME, name.data(),
                    name.size());
        }

        template <typename T1>
        void push_integral(T1 integral) {
            char bin[sizeof(T1)];

            memset(bin, 0, sizeof(T1)+1u);

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
        void start_scope() { this->push_empty_command(START_SCOPE); }
        void end_scope() { this->push_empty_command(END_SCOPE); }

        virtual size_t tell() {
            return this->offset;
        }

        virtual void seek(size_t to_pos) {
            this->offset = to_pos;
        }

        virtual ~CompileStream() {

        }
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
            memmove(dest, this->bin_data.data()+this->offset, size);

            this->offset += size;
        }

        size_t size() override {
            return this->bin_data.size();
        }
    };
}

#endif //KANLANG_STATEMENTS_CLASS_HPP
