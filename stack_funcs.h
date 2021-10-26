
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
    unsigned int hash_dat;
    canary_t canary_1;
    void* block_st;
    void* data;
    size_t element_sz;
    size_t size;
    size_t capacity;
    FILE *dump_fptr;
    canary_t canary_2;
    unsigned int hash_st;

} Stack;

enum DAT_COND {
    GOOD,
    BAD,
    INDEF,
};

enum ERRS {
    OK,
    STK_OK_ERR,
    NULL_POINTER,
    STK_OVERFLOW,
    BAD_SZ_CP,
    BAD_DAT,
    ALLOCATION_ERR,
    CNR_ERR,
    NO_ELEMENTS,
    NO_DUMP,
    BAD_ST_HASH,
    BAD_DAT_HASH,
};


ERRS StackCtor(Stack *st, size_t init_cp=0, size_t elem_sz=1, const char* fname="st_logs.html");//changes made

ERRS StackDtor(Stack *st);

ERRS StackOk(Stack *st, long long *errs_container=nullptr);

ERRS StackDmp(Stack *st, long long errs_container=0);

ERRS poison(Stack *st);

ERRS set_canaries(Stack *st);

ERRS check_canaries(Stack *st);

int set_bit(int bit_num, long long *container, int max_bite);

int get_bit(long long val, size_t bit_pos);

int is_poisoned(const Stack *st);

const char* colored(DAT_COND condition);

ERRS StackPop(Stack *st, void *buffer, long long *errs_container);

ERRS StackPush(Stack *st, void *value, long long *errs_container);

ERRS StackResize(Stack *st);

ERRS DumpInit(Stack *st, const char* fname="st_logs.html");

ERRS err_view(const Stack *st, long long errs_container);

ERRS table_text(FILE* fptr,const char* column1, const char* column2, const char* color);

ERRS DumpClose(Stack *st);

unsigned int hash_count(void* block_st, size_t length);

ERRS set_hash(Stack *st);

ERRS check_hash(Stack *st);

#endif //FIRST_STACK_STACK_FUNCS_H
