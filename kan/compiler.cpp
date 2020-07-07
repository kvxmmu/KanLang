//
// Created by kvxmmu on 7/4/20.
//

#include "compiler.hpp"

// DynamicByteBuffer

DynamicByteBuffer::DynamicByteBuffer(size_t initial_capacity) : capacity(initial_capacity) {
    this->buffer.reserve(initial_capacity);

    this->fill(initial_capacity);
}

void DynamicByteBuffer::fill(size_t count, char value) {
    for (size_t pos = 0; pos < count; pos++) {
        this->buffer.push_back(value);
    }
}

char &DynamicByteBuffer::operator[](size_t index) {
    if (index > this->capacity) {
        this->fill(index - this->capacity-1);
    }

    char &chr = this->buffer.at(index);

    return chr;
}

// IR Generator


void KanIR::call() {
    this->single_operand(CALL_COMMAND);
}

void KanIR::store(const std::string &name) {
    this->command_layer.push_back(std::string(STORE_COMMAND) + " " + name);
}

void KanIR::push(const std::string &type, const std::string &value) {
    this->command_layer.push_back(std::string(PUSH_COMMAND) + " " + type + " " + value);
}

void KanIR::notq() {
    this->single_operand(NOT_COMMAND);
}

void KanIR::single_operand(const std::string &operand) {
    this->command_layer.push_back(operand);
}

void KanIR::two_operands(const std::string &left, const std::string &right) {
    this->command_layer.push_back(left + " " + right);
}

void KanIR::orq() {
    this->single_operand(OR_COMMAND);
}

void KanIR::addq() {
    this->single_operand(ADD_COMMAND);
}

void KanIR::subq() {
    this->single_operand(SUB_COMMAND);
}

void KanIR::andq() {
    this->single_operand(AND_COMMAND);
}

void KanIR::incq() {
    this->single_operand(INC_COMMAND);
}

void KanIR::decq() {
    this->single_operand(DEC_COMMAND);
}

void KanIR::negq() {
    this->single_operand(NEG_COMMAND);
}

void KanIR::mulq() {
    this->single_operand(MUL_COMMAND);
}

void KanIR::divq() {
    this->single_operand(DIV_COMMAND);
}

void KanIR::modq() {
    this->single_operand(MOD_COMMAND);
}

void KanIR::load_attribute(const std::string &attribute) {
    this->two_operands(LOAD_ATTRIBUTE_COMMAND, attribute);
}

std::string KanIR::compile() {
    std::string out;

    for (auto &str : this->command_layer) {
        out += str + '\n';
    }

    out.pop_back();

    return out;
}
