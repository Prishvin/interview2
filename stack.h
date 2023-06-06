#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>
#include "types.h"

typedef struct
{
    BYTE* data;
    void* top;
    int count;
    int size;

} Stack;
BOOL stack_init(Stack* stack, int count, int size);
BOOL stack_push(Stack* stack, void* data);
void* stack_pop(Stack* stack);
BOOL stack_is_full(Stack* stack);


#endif