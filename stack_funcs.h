
#ifndef FIRST_STACK_STACK_FUNCS_H
#define FIRST_STACK_STACK_FUNCS_H

#include "globals.h"
#include "hash.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define POSITION __FILE__, __LINE__, __PRETTY_FUNCTION__
//Dear user, please choose poison for your type of data
typedef long long canary_t;

#define SECURITY_LVL 1





typedef struct {
    uint32_t hash_st;
    canary_t canary_1;
    void* block_st;
    void *data;
    size_t element_sz;
    size_t size;
    size_t capacity;
    FILE *dump_fptr;
    canary_t canary_2;
    uint32_t hash_dat;

} Stack;

enum DAT_COND {
    GOOD,
    BAD,
    INDEF,
};


ERRS StackCtor(Stack *st, size_t init_cp=0, size_t elem_sz=1);//changes made

ERRS StackDtor(Stack *st);

ERRS StackOk(const Stack *st, long long *errs_container);

ERRS StackDmp(const Stack *st, long long errs_container=0);

ERRS poison(Stack *st);

ERRS set_canaries(Stack *st);

ERRS check_canaries(const Stack *st);

int set_bit(int bit_num, long long *container, int max_bite);

int get_bit(long long val, size_t bit_pos);

int is_poisoned(const Stack *st);

const char* colored(DAT_COND condition);

ERRS StackPop(Stack *st, void *buffer, long long *errs_container);

ERRS StackPush(Stack *st, void *value, long long *errs_container);

ERRS StackResize(Stack *st);

ERRS DumpInit(Stack *st);

ERRS err_view(const Stack *st, long long errs_container);

ERRS table_text(FILE* fptr,const char* column1, const char* column2, const char* color);

ERRS DumpClose(const Stack *st);

#endif //FIRST_STACK_STACK_FUNCS_H
