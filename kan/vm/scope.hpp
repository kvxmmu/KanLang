//
// Created by kvxmmu on 3/7/21.
//

#ifndef KANLANG_SCOPE_HPP
#define KANLANG_SCOPE_HPP

#include <unordered_map>
#include <string>

#include "type.hpp"

namespace Kan::Memory {
    typedef std::string name_t;

    class Scope {
    public:
        std::unordered_map<name_t, Object *> names;

        bool has_name(const name_t &name) const;
        bool is_same_object(const name_t &name, Object *object) const;

        void set_name(const name_t &name, Object *object);
        void decref_name(const name_t &name);
        void incref_name(const name_t &name);

        void decref_names();
    };

    void deallocate_object(Object *object);

    template <typename CastType>
    static void KanObject_Free(Object *object) {
        delete reinterpret_cast<CastType *>(object);
    }
}


#endif //KANLANG_SCOPE_HPP
