
#ifndef FIRST_STACK_STACK_FUNCS_H
#define FIRST_STACK_STACK_FUNCS_H

#include "globals.h"
#include <limits.h>

#define LOCATION __FILE__, __LINE__, __PRETTY_FUNCTION__

typedef long long canary_t;
typedef long long element_t;
element_t DAT_POISON = 0xBADDED;


typedef struct {
    canary_t canary_1;
    element_t *data;
    size_t size;
    size_t capacity;
    canary_t canary_2;
} Stack;


ERRS StackCtor(Stack *st, size_t init_sz=0);

ERRS StackDtor(Stack *st);

ERRS StackOk(const Stack *st, long long *errs_container);

ERRS StackDmp(const Stack *st, long long errs_container=0);

int set_bit(int bit_num, long long *container, int max_bite);

int is_poisoned(const Stack *st);

#endif //FIRST_STACK_STACK_FUNCS_H
