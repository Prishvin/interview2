#include "stack.h"

// initialize the stack
BOOL stack_init(Stack* stack, int count, int size) {
    stack->data = (BYTE*) malloc(count * size);
    if(stack->data == NULL) return FALSE;
    stack->top = NULL;
    stack->count = 0;
    stack->size = size;
    return TRUE;
}

// push an element into the stack
BOOL stack_push(Stack* stack, void* data) {
    if(stack_is_full(stack)) return FALSE;
    BYTE* ptr = (BYTE*) stack->data + (stack->count * stack->size);
    memcpy(ptr, data, stack->size);
    stack->top = ptr;
    stack->count++;
    return TRUE;
}

BOOL stack_is_full(Stack* stack) {
    return (stack->count * stack->size >= sizeof(stack->data));
}

// pop an element from the stack
void* stack_pop(Stack* stack) {
    if(stack->count == 0) return NULL;
    void* data = stack->top;
    stack->count--;
    if(stack->count > 0) {
        stack->top = (BYTE*) stack->data + ((stack->count - 1) * stack->size);
    } else {
        stack->top = NULL;
    }
    return data;
}

// check if the stack is empty
BOOL stack_is_empty(Stack* stack) {
    return (stack->count == 0);
}
