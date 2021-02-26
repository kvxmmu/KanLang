//
// Created by kvxmmu on 2/26/21.
//

#ifndef KANLANG_LIST_OBJ_HPP
#define KANLANG_LIST_OBJ_HPP

#include "../type.hpp"

#include <vector>

namespace Kan::STD {
    using namespace Kan::VM;

    class ListObject : public Object {
    public:
        std::vector<Object *> objects;

        explicit ListObject(Type *_type) : Object(_type) {

        }

        [[nodiscard]] Object *at(size_t index) const {
            return this->objects.at(index);
        }
    };

    static Object *create_list_from_vector(const std::vector<Object *> &objects,
            Type *_type) {
        auto obj = new ListObject(_type);

        obj->objects.insert(obj->objects.begin(),
                objects.begin(), objects.end());

        return obj;
    }
}

#endif //KANLANG_LIST_OBJ_HPP
