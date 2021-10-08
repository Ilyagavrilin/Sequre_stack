
#ifndef FIRST_STACK_STACK_FUNCS_H
#define FIRST_STACK_STACK_FUNCS_H

#include "globals.h"
#include "hash.h"

#include <limits.h>
#include <stdint.h>

#define POSITION __FILE__, __LINE__, __PRETTY_FUNCTION__
//Dear user, please choose poison for your type of data
size_t DAT_POISON = 0xBADDED;

typedef long long canary_t;
size_t MARGIN_POISON = 0xBADDDED;
canary_t CNR_VAL = 0xB0BAB1BA;


typedef struct {
    uint32_t hash_st;
    canary_t canary_1;
    void* block_st;
    void *data;
    size_t element_sz;
    size_t size;
    size_t capacity;
    canary_t canary_2;
    uint32_t hash_dat;

} Stack;


ERRS StackCtor(Stack *st, size_t init_cp=0, size_t elem_sz=1);//changes made

ERRS StackDtor(Stack *st);

ERRS StackOk(const Stack *st, long long *errs_container);

ERRS StackDmp(const Stack *st, long long errs_container=0);

ERRS poison(Stack *st);

ERRS set_canaries(Stack *st);

ERRS check_canaries(Stack *st);

int set_bit(int bit_num, long long *container, int max_bite);

int is_poisoned(const Stack *st);

ERRS StackPop(Stack *st, void *buffer, long long *errs_container);

#endif //FIRST_STACK_STACK_FUNCS_H
