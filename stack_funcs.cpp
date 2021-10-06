#include "globals.h"
#include "stack_funcs.h"



 ERRS StackCtor(Stack *st, size_t init_sz) {
    long long errs_container = 0;
    ERRS res = OK;
    if ((res = StackOk(st, &errs_container)) != OK) {
        fprintf(stderr, "Error in %s in line %d, function %s: "
                        "Can`t initialize stack with non available address.\n", LOCATION);
        StackDmp(st, errs_container);
        return res;

    }
    st->data =  (element_t*) calloc(st->capacity, sizeof(element_t)) ;

    if (st->data == nullptr) {
        fprintf(stderr, "StackCtor():Can`t initialize memory for stack data (no free memory).\n");
        return NULL_POINTER ;
    }

    return OK;
}

ERRS StackDtor(Stack *st) {
    if (st == nullptr) {
        fprintf(stderr, "StackDtor():Can`t destroy stack with non valid address.\n");
        return NULL_POINTER;

    }

    free(st->data);
    st->size = DAT_POISON;
    st->capacity = DAT_POISON;

    return OK;
}

ERRS StackPop(Stack *st, void *buffer, long long *errs_container) {
     ERRS last_err = OK;
     if ((last_err = StackOk(st, errs_container)) != OK) {
         return last_err;
     }

     memcpy(buffer, st->data, 10);
     st->size--;
     return last_err;
 }

int is_poisoned(const Stack *st) {
    for (size_t i = st->size; i < st->capacity; i++) {
        if (st->data[i] != DAT_POISON) {
            return 0;
        }
    }
    return 1;
 }

 int set_bit(int bit_num, long long *container,int max_bite) {
     if (bit_num > (max_bite * CHAR_BIT - 1)) {
        return -1;
     }
     *container = *container | (1 << bit_num);

     return 0;
 }

/*
 * we write err code like turning some bits in long long into 1
 * 0 bit - non valid pointer to stack
 * 1 bit - non valid pointer to massive
 * 2 bit - error in stack`s capacity or size
 * 3 bit - some movements changed information in stack buffer
 */
ERRS StackOk(const Stack *st, long long *errs_container) {
    ERRS last_err = OK;
    if (*errs_container != 0ll) {
        return STK_OK_ERR;
    }
    if (st == nullptr) {
        set_bit(0, errs_container, sizeof(long long));
        fprintf(stderr, "StackOk():You try to check stack with non valid pointer.\n");
        return NULL_POINTER;
    }

    if (st->data == nullptr) {
        set_bit(1, errs_container, sizeof(long long));
        last_err = BAD_DAT;
    }
    if (st->capacity < st->size) {
        last_err = BAD_SZ_CP;
        set_bit(2, errs_container, sizeof(long long));
    }
    if (is_poisoned(st) == 0) {
        last_err = BAD_DAT;
        set_bit(3, errs_container, sizeof(long long));
    }

    return last_err;
}
