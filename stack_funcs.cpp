#include "globals.h"
#include "stack_funcs.h"


ERRS StackCtor(Stack *st, size_t init_cp, size_t elem_sz) {
    //check which pointer had been given from user
    if (st == nullptr) {
        fprintf(stderr, "Error in %s in line (%d), function %s: "
                        "StackOk() returned non ok status.\n", POSITION);
        return NULL_POINTER;
    }

    //set canaries in stack structure
    st->canary_1 = CNR_VAL;
    st->canary_2 = CNR_VAL;

    //add initial params to stack fields
    st->size = 0;
    st->element_sz = elem_sz;

    //initialize dynamic memory and set her value to poison and canaries
    st->capacity = init_cp;
    //stop initialization if malloc can`t find needed block of memory
    if((st->block_st = malloc((init_cp * elem_sz) + (2 * sizeof(canary_t)))) == nullptr){
        StackDtor(st);
        return ALLOCATION_ERR;
    }
    st->data = (void*)((canary_t*)(st->block_st) + 1);
    set_canaries(st);
    poison(st);

    //check accuracy of stack construction
    long long errs_container = 0;
    ERRS res = OK;
    if ((res = StackOk(st, &errs_container)) != OK) {
        fprintf(stderr, "Error in %s in line (%d), function %s: "
                        "StackOk() returned non ok status.\n", POSITION);
        StackDmp(st, errs_container);

    }

    return res;
}

ERRS StackDtor(Stack *st) {
    if (st == nullptr) {
        fprintf(stderr, "StackDtor():Can`t destroy stack with non valid address.\n");
        return NULL_POINTER;

    }

    long long errs_container = 0;
    ERRS res = OK;
    if ((res = StackOk(st, &errs_container)) != OK) {
        fprintf(stderr, "Error in %s in line (%d), function %s: "
                        "StackOk() returned non OK status.\n", POSITION);
        StackDmp(st, errs_container);

    }

    st->element_sz = MARGIN_POISON;
    st->capacity = MARGIN_POISON;
    st->size = MARGIN_POISON;

    free(st->block_st);
    st->block_st = nullptr;
    st->data = nullptr;


    return res;
}

//no security, because nun nullptr *st was checked previously in StackCtor() or int StackResize()
ERRS poison(Stack *st) {
    for (size_t i = st->size; i < st->capacity; i++) {
        void* destination = (void*) ((char*)st->data + st->element_sz * i);
        memcpy(destination, &DAT_POISON, st->element_sz);
    }
    return OK;
}

int is_poisoned(const Stack *st) {
    for (size_t i = st->size; i < st->capacity; i++) {
        void* position = (void*) ((char*)st->data + st->element_sz * i);
        if (memcmp(position, &DAT_POISON, st->element_sz) != 0) {
            return 0;
        }
    }
    return 1;
}

int set_bit(int bit_num, long long *container, int max_bite) {
    if (bit_num > (max_bite * CHAR_BIT - 1)) {
        return -1;
    }
    *container = *container | (1 << bit_num);

    return 0;
}

/*
 * we write err code like turning some bits in long long into 1
 * 0 bit - non valid pointer to stack(stops work of stackOk)
 * 1 bit - non valid pointer to massive
 * 2 bit - error in stack`s capacity or size
 * 3 bit - some movements changed information in stack buffer
 * 4 bit - canaries value had been changed
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
    if (check_canaries(st) != OK) {
        last_err = CNR_ERR;
        set_bit(4, errs_container, sizeof(long long));
    }

    return last_err;
}

ERRS set_canaries(Stack *st) {
    //TODO: add stackOk or another security system
    ((canary_t*) st->data)[-1] = CNR_VAL;
    *(canary_t*)((char*)st->data + st->capacity * st->element_sz) = CNR_VAL;

    return OK;
}

ERRS check_canaries(const Stack *st) {
    ERRS res = OK;
    //check canaries in stack struct
    int is_error = 0;
    if (st->canary_1 != CNR_VAL || st->canary_2 != CNR_VAL) {
        char* pos_cnry;
        if (st->canary_1 != CNR_VAL) {
            pos_cnry = "left";
        }
        else {
            pos_cnry = "right";
        }

        StackDmp(st);
        fprintf(stderr, "in file %s(%d) function: %s. Changes value of %s canary in stack, please, check dump.\n",
                POSITION, pos_cnry);
        is_error = 1;
        res = CNR_ERR;
    }
    //check canaries in data
    if ((((canary_t*) st->data)[-1] != CNR_VAL) || (*(canary_t*)((char*)st->data + st->capacity * st->element_sz) != CNR_VAL)) {
        char * pos_cnry;
        if (((canary_t*) st->data)[-1] != CNR_VAL) {
            pos_cnry = "left";
        }
        else{
            pos_cnry = "right";
        }
        fprintf(stderr, "in file %s(%d) function: %s. Changes value of %s canary in stack, please, check dump.\n",
                POSITION, pos_cnry);
        is_error = 1;
    }

    if (is_error == 1) {
        res = CNR_ERR;
        StackDmp(st);
    }
    return res;
}


ERRS StackPop(Stack *st, void *buffer, long long *errs_container) {
    ERRS last_err = OK;
    if ((last_err = StackOk(st, errs_container)) != OK) {
        //finish process, because we can`t work with non initialized stack
        if (last_err == NULL_POINTER) {return last_err;}
    }
    //check if stack has element to pop
    if (st->size == 0) {
        return NO_ELEMENTS;
    }
    //copy last element to user`s buffer
    void* pointer = (char*)st->data + st->size * st->element_sz;
    memcpy(buffer, pointer, st->element_sz);
    //use single poison by memcpy(), because we need poison only one element
    memcpy(pointer, &DAT_POISON, st->element_sz);

    st->size--;
    StackResize(st);

    return last_err;
}

ERRS StackPush(Stack *st, void *value, long long *errs_container) {
    ERRS last_err = OK;
    if ((last_err = StackOk(st, errs_container)) != OK) {
        if (last_err == NULL_POINTER) {return last_err;}
    }
    st->size++;
    StackResize(st);

    void* pointer = (char*)st->data + st->size * st->element_sz;
    memcpy(pointer, value, st->element_sz);

    return last_err;
}


ERRS StackResize(Stack *st) {
    double rsz_coef_more = 2;
    double rez_coef_less = 0.75;
    //choose how change resize coefficient on big sizes of stack
    if (st->capacity > 1000) {
        rsz_coef_more = 1.8;
    }
    else if (st->capacity > 100000) {
        rsz_coef_more = 1.6;
    }


    if (st->size < st->capacity) {

    }
}