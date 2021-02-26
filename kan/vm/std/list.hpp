//
// Created by kvxmmu on 2/25/21.
//

#ifndef KANLANG_LIST_HPP
#define KANLANG_LIST_HPP

#include "../type.hpp"
#include "list_obj.hpp"
#include "int.hpp"

#include <vector>

namespace Kan::STD {
    using namespace Kan::VM;

    class ListType : public Type {
    public:
        ListType() {
            this->getitem = [](Object *args) {
                auto arglist = reinterpret_cast<ListObject *>(args);
                auto self = reinterpret_cast<ListObject *>(arglist->at(0));
                auto item_index = reinterpret_cast<IntObject *>(arglist->at(1));

                return self->at(item_index->value);
            };
        }

        DEFAULT_FREE_FUNC(ListObject)
        DEFAULT_IS_SAME_FUNC()

        name_t get_type_name() override {
            return CLASSNAME;
        }
    };
}

#endif //KANLANG_LIST_HPP
