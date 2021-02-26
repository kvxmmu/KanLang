//
// Created by kvxmmu on 2/25/21.
//

#ifndef KANLANG_STACK_H
#define KANLANG_STACK_H

#include <stdlib.h>

typedef struct {
    void *stack;

    size_t capacity;
    size_t length;
    size_t ptr;

    size_t object_size;
} Stack;

Stack stack_create(size_t object_size, size_t capacity) {
    Stack stack;

    stack.stack = calloc(capacity, object_size);
    stack.capacity = capacity;
    stack.object_size = object_size;
    stack.ptr = 0;
    stack.length = 0;

    return stack;
}

void stack_twice_size(Stack *stack) {
    stack->stack = realloc(stack->stack, stack->object_size*(stack->capacity<<1u));
    stack->capacity <<= 1u;
}

void stack_expand(Stack *stack, size_t until) {
    while (stack->capacity < until) {
        stack_twice_size(stack);
    }
}

void stack_pop(Stack *stack, void *dest) {
    void *stackptr = ((char *)stack->stack)+(stack->length*stack->object_size)-stack->object_size;

    memcpy(dest, stackptr, stack->object_size);
    stack->length--;
}

void stack_push(Stack *stack, void *data) {
    if (stack->capacity < stack->length+1u) {
        stack_expand(stack, stack->length+1u);
    }

    memcpy((char *)stack->stack+(stack->length*stack->object_size),
            data, stack->object_size);

    stack->length++;
}

void stack_free(Stack *stack) {
    free(stack->stack);
}

#endif //KANLANG_STACK_H
