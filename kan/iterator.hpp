//
// Created by kvxmmu on 2/12/21.
//

#ifndef KANLANG_ITERATOR_HPP
#define KANLANG_ITERATOR_HPP

namespace Kan {
    template <typename T,
            typename V = typename T::value_type>
    class Iterator {
    public:
        size_t pos;

        T &container;
        V default_value;

        explicit Iterator(T &_container,
                V _default_value = V(),
                size_t start_from = 0) : pos(start_from), container(_container), default_value(_default_value) {

        }

        [[nodiscard]] constexpr bool is_done(size_t offset) {
            return (this->pos+offset) >= this->container.size();
        }

        const V &peek(size_t offset = 0) {
            if (this->is_done(offset)) {
                return this->default_value;
            }

            return this->container[this->pos + offset];
        }

        constexpr void next(size_t offset = 1) {
            this->pos += offset;
        }
    };
}

#endif //KANLANG_ITERATOR_HPP
