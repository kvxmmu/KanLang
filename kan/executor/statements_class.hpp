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
#include <iostream>

namespace Kan::Statements {
    typedef std::string name_t;

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
        virtual void replace_data(const char *_data, size_t size) = 0;
        virtual const char *get_buffer() = 0;

        virtual size_t size() { return 0; }

        void push_empty_command(uint8_t command_type) {
            this->write_integral<uint8_t>(command_type);
        }

        template <typename Integral>
        void push_command_with_arg(uint8_t command_type, Integral integral) {
            this->write_integral<uint8_t>(command_type);
            this->write_integral<Integral>(integral);
        }

        template <typename Integral>
        void replace_integral(Integral integral) {
            char bin[sizeof(Integral)];

            int_to_bytes<Integral>(integral, bin);

            this->replace_data(bin, sizeof(Integral));
        }

        void push_sized_command(uint8_t command_type, const char *_data,
                                uint16_t size = 0) {
            this->write_integral<uint8_t>(command_type);
            this->write_integral<uint16_t>(size);
            this->write_to_stream(_data, size);
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

        void set_name(const name_t &name) {
            this->push_sized_command(SET_NAME, name.data(),
                    name.size());
        }

        void push_buffered_command(uint8_t type, char *buffer, size_t bufflen) {
            this->write_integral<uint8_t>(type);
            this->write_to_stream(buffer, bufflen);
        }

        template <typename T1>
        void push_integral(T1 integral) {
            char bin[1+sizeof(T1)];

            memset(bin, 0, sizeof(T1)+1u);

            int_to_bytes<T1>(integral, bin+1);
            bin[0] = static_cast<uint8_t>(std::is_same<T1, float>::value);

            this->push_buffered_command(PUSH_INTEGRAL, bin,
                                        sizeof(T1)+1);
        }

        void plus() { this->push_empty_command(PLUSB); }
        void minus() { this->push_empty_command(MINUSB); }
        void pow() { this->push_empty_command(POWB); }
        void div() { this->push_empty_command(DIVB); }
        void mul() { this->push_empty_command(MULB); }
        void call(uint8_t argscount) { this->push_command_with_arg(CALLB, argscount); }
        void uminus() { this->push_empty_command(UMINUS); }
        void uplus() { this->push_empty_command(UPLUS); }
        void start_scope() { this->push_empty_command(START_SCOPE); }
        void end_scope() { this->push_empty_command(END_SCOPE); }
        void jmp_if_false(uint32_t _offset) { this->push_command_with_arg(JMP_IF_FALSE, _offset); }
        void pop_object() { this->push_empty_command(POP_OBJECT); }
        void else_block(uint32_t _offset) {
            this->replace_last_empty_command(JMP_IF_TRUE);
            this->write_integral<uint32_t>(_offset);
        }
        void jmp_to_address(uint64_t address) {
            this->push_command_with_arg(JMP_TO_ADDRESS, address);
        }
        void ret() { this->push_empty_command(RET); }
        void ret_null() { this->push_empty_command(RET_NULL); }
        void push_true() { this->push_empty_command(PUSH_TRUE); }
        void push_false() { this->push_empty_command(PUSH_FALSE); }
        void push_none() { this->push_empty_command(PUSH_NONE); }
        void left_shift() { this->push_empty_command(LEFT_SHIFTB); }
        void right_shift() { this->push_empty_command(RIGHT_SHIFTB); }
        void bin_or() { this->push_empty_command(BIN_ORB); }
        void bin_and() { this->push_empty_command(BIN_ANDB); }
        void bin_xor() { this->push_empty_command(BIN_XORB); }
        void bin_not() { this->push_empty_command(BIN_NOTB); }

        void define_function(uint32_t length, const std::string &name,
                uint8_t argscount) {
            this->push_command_with_arg<uint32_t>(DEFINE_FUNCTION, length);
            this->write_integral<uint8_t>(name.size());
            this->write_integral<uint8_t>(argscount);
            this->write_to_stream(name.c_str(), name.size());
        }

        void create_class(const std::string &name,
                          uint64_t length) {
            this->write_integral<uint8_t>(CREATE_TYPE);
            this->write_integral<uint8_t>(name.size());
            this->write_integral<uint64_t>(length);
            this->write_to_stream(name.c_str(), name.size());
        }

        void add_class_method(const std::string &name, uint32_t length, uint8_t argscount) {
            this->write_integral<uint8_t>(name.size());
            this->write_integral<uint32_t>(length);
            this->write_integral<uint8_t>(argscount);
            this->write_to_stream(name.c_str(), name.size());
        }

        void add_argument_signature(const std::string &name) {
            this->write_integral<uint8_t>(name.size());
            this->write_to_stream(name.c_str(), name.size());
        }

        std::string read_argument_signature() {
            auto length = this->read_integral<uint8_t>();
            auto str = this->get_string(length);

            return str;
        }

        void get_from_stack_and_assign(const std::string &name) {
            this->write_integral<uint8_t>(GET_FROM_STACK_AND_ASSIGN);
            this->write_integral<uint8_t>(name.size());
            this->write_to_stream(name.c_str(), name.size());
        }

        std::vector<std::string> read_signature(uint8_t argscount) {
            std::vector<std::string> sig;

            for (uint8_t pos = 0; pos < argscount; pos++) {
                sig.push_back(this->read_argument_signature());
            }

            return sig;
        }

        void lesser() { this->push_empty_command(LESSERB); }
        void greater() { this->push_empty_command(GREATERB); }
        void lesser_or_equal() { this->push_empty_command(LESSER_OR_EQUALB); }
        void greater_or_equal() { this->push_empty_command(GREATER_OR_EQUALB); }
        void equal() { this->push_empty_command(EQUALB); }

        void replace_last_empty_command(uint8_t to) {
            this->seek(this->tell()-1u);

            this->replace_integral<uint8_t>(to);
        }

        void copy_stream_buffer(CompileStream *_stream) {
            char buff[4096]; // 4kb copy buffer
            size_t sz = _stream->count();

            while (sz > 0) {
                size_t block = 4096;

                if (sz < block) {
                    block = sz;
                }

                _stream->read_from_stream(buff, block);

                sz -= block;

                this->write_to_stream(buff, block);
            }
        }

        virtual size_t tell() {
            return this->offset;
        }

        virtual size_t count() {
            return this->size() - this->offset;
        }

        virtual void seek(size_t to_pos) {
            this->offset = to_pos;
        }

        virtual ~CompileStream() = default;
    };

    class VectorCompileStream : public CompileStream {
    public:
        std::vector<unsigned char> bin_data;

        void write_to_stream(const char *_data, size_t size) override {
            this->bin_data.insert(this->bin_data.begin()+this->offset,
                                  _data, _data+size);

            this->offset += size;
        }

        void replace_data(const char *_data, size_t size) override {
            for (size_t pos = 0; pos < size; pos++) {
                this->bin_data[pos+this->offset] = _data[pos];
            }

            this->offset += size;
        }

        void read_from_stream(char *dest, size_t size) override {
            memmove(dest, this->bin_data.data()+this->offset, size);

            this->offset += size;
        }

        const char * get_buffer() override {
            return reinterpret_cast<const char *>(this->bin_data.data());
        }

        size_t size() override {
            return this->bin_data.size();
        }
    };
}

#endif //KANLANG_STATEMENTS_CLASS_HPP
