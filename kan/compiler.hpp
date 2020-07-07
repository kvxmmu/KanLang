//
// Created by kvxmmu on 7/4/20.
//

#ifndef KANLANG_COMPILER_HPP
#define KANLANG_COMPILER_HPP

#include <string>
#include <sstream>
#include <vector>

#define CALL_COMMAND "call"
#define STORE_COMMAND "store"
#define PUSH_COMMAND "push"

#define NOT_COMMAND "not"
#define OR_COMMAND "or"
#define AND_COMMAND "and"
#define ADD_COMMAND "add"
#define SUB_COMMAND "sub"
#define INC_COMMAND "inc"
#define DEC_COMMAND "dec"
#define NEG_COMMAND "neg"
#define MUL_COMMAND "mul"
#define DIV_COMMAND "div"
#define MOD_COMMAND "mod"
#define LOAD_ATTRIBUTE_COMMAND "load_attr"

class DynamicByteBuffer {
public:
    std::vector<char> buffer;
    size_t capacity;

    explicit DynamicByteBuffer(size_t initial_capacity);

    void fill(size_t count, char value = 0);

    char &operator[](size_t index);
};


class KanBytecodeCompiler {
public:
    // knbc
};

class KanIR {
public:
    std::vector<std::string> command_layer;

    void single_operand(const std::string &operand);
    void two_operands(const std::string &left, const std::string &right);

    void store(const std::string &name);
    void push(const std::string &type, const std::string &value);

    void notq();
    void orq();
    void andq();

    void addq();
    void subq();
    void mulq();
    void divq();
    void modq();

    void incq();
    void decq();
    void negq();

    void load_attribute(const std::string &attribute);

    void call();

    std::string compile();
};


#endif //KANLANG_COMPILER_HPP
