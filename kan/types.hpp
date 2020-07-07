//
// Created by kvxmmu on 7/3/20.
//

#ifndef KANLANG_TYPES_HPP
#define KANLANG_TYPES_HPP


#include <cstddef>
#include <cinttypes>

#include <string>
#include <vector>
#include <iostream>

#include <type_traits>

//////////////

#define NO_TYPE 0
#define NUMBER 1

#define DOT 2
#define PLUS 3
#define MINUS 4
#define MOD 5
#define POW 6
#define MUL 7
#define EQUALS 8
#define NOT_EQUALS 9
#define NOT 10
#define GREATER_OR_EQUALS 11
#define LESSER_OR_EQUALS 12
#define ID 14
#define STRING 15
#define OPEN_BRACKET 16
#define CLOSE_BRACKET 17
#define OPEN_QBRACKET 18
#define CLOSE_QBRACKET 19
#define OPEN_BRACE 20
#define CLOSE_BRACE 21
#define DIV 22

#define OPERATOR 13

#define NORMAL_PRIORITY 1
#define HALF_PRIORITY 2
#define FULL_PRIORITY 0xfu
#define DO_FIRST_PRIORITY 0xffu


#define TOKEN 1
#define TREE 2

/////////////

#define PARSE_ERROR "ParseError"
#define SHOW_CODE_LEN 20

typedef uint16_t token_t;
typedef uint8_t priority_t;
typedef uint8_t ast_object_type_t;

template <typename T,
        typename VT = typename T::value_type>
class PeekableIterator {
public:
    size_t pos = 0;
    size_t size = 0;
    T &container;

    VT default_response = VT();

    explicit PeekableIterator(T &_container) : container(_container) {
        this->size = _container.size();
    }

    bool is_done(size_t step = 0) {
        return this->pos + step >= this->size;
    }

    VT &peek(size_t step) {
        if (this->is_done(step)) {
            return this->default_response;
        }

        return this->container.at(this->pos + step);
    }

    void next(size_t step = 1) {
        this->pos += step;
    }
};

template <typename T,
        typename VT = typename T::value_type>
class ConstPeekableIterator {
public:
    size_t pos = 0;
    size_t size = 0;
    const T &container;

    VT default_response = VT();

    explicit ConstPeekableIterator(const T &_container) : container(_container) {
        this->size = _container.size();
    }

    bool is_done(size_t step = 0) {
        return this->pos + step >= this->size;
    }

    const VT &peek(size_t step) {
        if (this->is_done(step)) {
            return this->default_response;
        }

        return this->container.at(this->pos + step);
    }

    void next(size_t step = 1) {
        this->pos += step;
    }
};

typedef struct {
    std::string text;
    token_t subtype;

    priority_t priority = NORMAL_PRIORITY;
} Operator;

class Token {
public:
    std::string value;

    token_t type{};
    token_t subtype{};
    priority_t priority{};

    unsigned int line = 1;
    unsigned int char_pos = 0;

    uint16_t flags = 0;

    Token(std::string _value, token_t _type,
            priority_t _priority = NORMAL_PRIORITY,
            token_t _subtype = NO_TYPE);

    Token() = default;

    friend std::ostream &operator<<(std::ostream &stream, const Token &token) {
        stream << "Token{type=" << static_cast<int>(token.type) << ", ";
        stream << "subtype=" << static_cast<int>(token.subtype) << ", ";
        stream << "priority=" << static_cast<int>(token.priority) << ", ";
        stream << "line=" << token.line << ", ";
        stream << "char_pos=" << token.char_pos << ", ";
        stream << "text=" << token.value << "}";

        return stream;
    }

    inline bool has_flag(uint16_t flag) {
        return (this->flags & flag) == flag;
    }
};

class AstTree;

class AstObject {
public:
    AstTree *top = nullptr;

    AstTree *tree = nullptr;
    Token token;

    ast_object_type_t type = TOKEN;

    AstObject(AstTree *_top, AstTree *_tree);
    AstObject(AstTree *_top, Token _token);
};

class AstTree {
public:
    std::vector<AstObject> objects;

    [[maybe_unused]] typedef AstObject value_type;

    template <typename ...Args>
    void emplace_back(Args&&... args);

    template <typename ...Args>
    [[maybe_unused]] void emplace_front(Args&& ...args);

    [[nodiscard]] [[maybe_unused]] std::size_t size() const noexcept;

    AstObject &operator[](size_t index);
    AstObject &at(size_t index);

};

template<typename... Args>
void AstTree::emplace_back(Args&&... args) {
    this->objects.emplace_back(this, std::forward<Args>(args)...);
}

template<typename... Args>
[[maybe_unused]]
void AstTree::emplace_front(Args&&... args) {
    this->objects.emplace(this->objects.begin(), this, std::forward<Args>(args)...);
}


#endif //KANLANG_TYPES_HPP
