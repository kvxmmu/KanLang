//
// Created by kvxmmu on 7/3/20.
//

#include "types.hpp"

Token::Token(std::string _value, token_t _type,
        priority_t _priority,
        token_t _subtype)  : value(std::move(_value)), type(_type), subtype(_subtype),
                             priority(_priority) {

}

AstObject::AstObject(AstTree *_top, AstTree *_tree) : top(_top), tree(_tree) {
    this->type = TREE;
}

AstObject::AstObject(AstTree *_top, Token _token) : top(_top), token(std::move(_token)) {

}

std::size_t AstTree::size() const noexcept {
    return this->objects.size();
}

AstObject &AstTree::operator[](size_t index) {
    return this->at(index);
}

AstObject &AstTree::at(size_t index) {
    return this->objects.at(index);
}
