//
// Created by kvxmmu on 7/3/20.
//

#include "types.hpp"

Token::Token(std::string _value, token_t _type,
        priority_t _priority,
        token_t _subtype)  : value(std::move(_value)), type(_type), subtype(_subtype),
                             priority(_priority) {

}

AstObject::AstObject(AstTree *_top, AstTree *_tree) : top(_top), tree(_tree),
                                                        priority(_tree->priority) {
    this->type = TREE;
}

AstObject::AstObject(AstTree *_top, Token _token) : top(_top), token(std::move(_token)),
        priority(_token.priority) {

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

void AstTree::erase(const std::vector<AstObject>::iterator &first, const std::vector<AstObject>::iterator &last) {
    this->objects.erase(first, last);
}

void AstTree::erase(const std::vector<AstObject>::iterator &start) {
    this->objects.erase(start);
}

void AstTree::free() {
    for (auto &object : this->objects) {
        if (object.type == TREE) {
            auto child = object.tree;

            child->free();
        }
    }

    if (this->need_free) {
        delete this;
    }
}

void AstTree::replace(size_t start, size_t size, const Token &token) {
    AstObject object(this, token);

    this->objects.erase(this->objects.begin()+start, this->objects.begin()+start+size);
    this->objects.insert(this->objects.begin()+start, object);
}


