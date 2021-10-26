#include "globals.h"
#include "stack_funcs.h"

size_t DAT_POISON = 0xBADDED;
size_t MARGIN_POISON = 0xBADDDED;
canary_t CNR_VAL = 0xB0BAB1BA;


size_t STACK_SZ = sizeof(Stack) - sizeof(unsigned int);

int ERR_CNT = 6;

ERRS StackCtor(Stack *st, size_t init_cp, size_t elem_sz, const char* fname) {
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
    if ((st->block_st = malloc((init_cp * elem_sz) + (2 * sizeof(canary_t)))) == nullptr) {
        StackDtor(st);
        return ALLOCATION_ERR;
    }
    st->data = (void *) ((canary_t *) (st->block_st) + 1);
    set_canaries(st);
    poison(st);
    set_hash(st);

    if (SECURITY_LVL > 0) {
        DumpInit(st, fname);
    }
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
    DumpClose(st);


    return res;
}

//no security, because nun nullptr *st was checked previously in StackCtor() or int StackResize()
ERRS poison(Stack *st) {
    for (size_t i = st->size; i < st->capacity; i++) {
        void *destination = (void *) ((char *) st->data + st->element_sz * i);
        memcpy(destination, &DAT_POISON, st->element_sz);
    }
    return OK;
}

int is_poisoned(const Stack *st) {
    for (size_t i = st->size; i < st->capacity; i++) {
        void *position = (void *) ((char *) st->data + st->element_sz * i);
        if (memcmp(position, &DAT_POISON, st->element_sz) != 0) {
            return 0;
        }
    }
    return 1;
}

int set_bit(int bit_num, long long *container, int max_bite) {
    if (container == nullptr) {
        return NULL_POINTER;
    }
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
 * 5 bit - hash value changed
 */
ERRS StackOk(Stack *st, long long *errs_container) {
    ERRS last_err = OK;
    ERRS res = OK;
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
    if ((res = check_hash(st)) != OK) {
        last_err = res;
        set_bit(5, errs_container, sizeof(long long));
    }

    return last_err;
}

ERRS table_text(FILE *fptr, const char *column1, const char *column2, const char *color) {
    if (fptr == nullptr) {
        return NULL_POINTER;
    }

    fprintf(fptr, "<tr> <td><font color = %s >%s</font></td> "
                  "<td><font color = %s >%s</font></td></tr>\n", color, column1, color, column2);
    return OK;
}

ERRS err_view(const Stack *st, long long errs_container) {
    const char *err = nullptr;
    const char *color = nullptr;
    fprintf(st->dump_fptr, "<table align=center>\n");
    for (int i = 0; i <= ERR_CNT; i++) {

        if (get_bit(errs_container, i) == 0) {
            err = "OK";
            color = colored(GOOD);
        } else {
            err = "ERROR";
            color = colored(BAD);
        }

        const char *err_defin = nullptr;
        switch (i) {
            case 0:
                err_defin = "Pointer to stack structure";
                break;
            case 1:
                err_defin = "Pointer to massive with stack data";
                break;
            case 2:
                err_defin = "Capacity and size values";
                break;
            case 3:
                err_defin = "Data integrity";
                break;
            case 4:
                err_defin = "Canaries values";
                break;
            case 5:
                err_defin = "Hash value";
                break;
            default:
                err_defin = "Unkown type of error";
                break;
        }
        table_text(st->dump_fptr, err_defin, err, color);
    }
    fprintf(st->dump_fptr, "</table>\n");
    return OK;
}

ERRS set_canaries(Stack *st) {
    ((canary_t *) st->data)[-1] = CNR_VAL;
    *(canary_t *) ((char *) st->data + st->capacity * st->element_sz) = CNR_VAL;

    return OK;
}

ERRS check_canaries(Stack *st) {
    ERRS res = OK;
    //check canaries in stack struct
    int is_error = 0;
    if (st->canary_1 != CNR_VAL || st->canary_2 != CNR_VAL) {
        const char *pos_cnry;
        if (st->canary_1 != CNR_VAL) {
            pos_cnry = "left";
        } else {
            pos_cnry = "right";
        }

        StackDmp(st);
        fprintf(stderr, "in file %s(%d) function: %s. Changes value of %s canary in stack, please, check dump.\n",
                POSITION, pos_cnry);
        is_error = 1;
        res = CNR_ERR;
    }
    //check canaries in data
    if ((((canary_t *) st->data)[-1] != CNR_VAL) ||
        (*(canary_t *) ((char *) st->data + st->capacity * st->element_sz) != CNR_VAL)) {
        const char *pos_cnry;
        if (((canary_t *) st->data)[-1] != CNR_VAL) {
            pos_cnry = "left";
        } else {
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
        if (last_err == NULL_POINTER) { return last_err; }
    }
    //check if stack has element to pop
    if (st->size == 0) {
        return NO_ELEMENTS;
    }
    //copy last element to user`s buffer
    void *pointer = (char *) st->data + st->size * st->element_sz;
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
        if (last_err == NULL_POINTER) { return last_err; }
    }
    st->size++;
    StackResize(st);

    void *pointer = (char *) st->data + st->size * st->element_sz;
    memcpy(pointer, value, st->element_sz);

    return last_err;
}

ERRS StackResize(Stack *st) {
    double rsz_coef_more = 2;
    double rsz_coef_less = 0.8;
    double gisteresis = 0.5;
    //choose how change resize coefficient on big sizes of stack
    if (st->capacity > 1000) {
        rsz_coef_more = 1.8;
    } else if (st->capacity > 100000) {
        rsz_coef_more = 1.6;
    }


    if (st->size < st->capacity) {
        if (st->size < (int) (st->capacity * gisteresis)) {
            void *pointer = nullptr;
            pointer = realloc(st->block_st, (int) (st->capacity * rsz_coef_less) + 2 * sizeof(canary_t));
            if (pointer == nullptr) {
                return NULL_POINTER; //do not change stack block_st pointer, work with more size
            }
            else {
                st->capacity = (size_t) (st->capacity * rsz_coef_less);
                st->block_st = pointer;
                st->data = (void*) ((canary_t*) pointer + 1);

            }
        }

    }
    if (st->size > st->capacity) {
        void *pointer = nullptr;
        pointer = realloc(st->block_st, (int) (st->capacity * rsz_coef_more));
        if (pointer == nullptr) {
            return NULL_POINTER; //do not change stack block_st pointer, work with more size
        }
        else {
            st->capacity = (size_t) (st->capacity * rsz_coef_more);
            st->block_st = pointer;
            st->data = (void*) ((canary_t*) pointer + 1);
            poison(st);

        }
    }
    set_canaries(st);
    set_hash(st);
    return OK;
}

const char *colored(DAT_COND condition) {
    if (condition == GOOD) {
        return "#008000";
    } else if (condition == BAD) {
        return "#FF0000";
    } else if (condition == INDEF) {
        return "#FFD700";
    } else {
        return nullptr;
    }
}

int get_bit(long long val, size_t bit_pos) {
    return (int) ((val >> bit_pos) & 1ll);
}

ERRS DumpInit(Stack *st, const char* fname) {
    if (st == nullptr) {
        fprintf(stderr, "Error in %s in line (%d), function %s: "
                        "DumpInit() returned non ok status.\n", POSITION);
        return NULL_POINTER;
    }
    FILE *fl_ptr = nullptr;
    switch (SECURITY_LVL) {
        case 0:
            return NO_DUMP;
        default:
            fl_ptr = fopen(fname, "w");
    }

    if (fl_ptr == nullptr) {
        return NULL_POINTER;
    }
    st->dump_fptr = fl_ptr;

    time_t curr_time = time(NULL);
    fprintf(fl_ptr, "<!DOCTYPE html>\n"
                    "<html lang=\"en\">\n"
                    "<head>\n"
                    "   <meta charset=\"UTF-8\">\n"
                    "   <title>Secure stack dump</title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<p align=\"center\"><img src=\"/kit.jpg\" height=\"256\"></p>\n"
                    "<p align=center>Dump initialized at %s</p>\n", asctime(gmtime(&curr_time)));
    return OK;
}

ERRS DumpClose(Stack *st) {
    if (st->dump_fptr == nullptr) {
        return NO_DUMP;
    }
    fprintf(st->dump_fptr, "<p align=center>Dump finished, file closed.</p>\n"
                           "</body>\n"
                           "</html>");
    return OK;
}

/*
 * we write err code like turning some bits in long long into 1
 * 0 bit - non valid pointer to stack(stops work of stackOk)
 * 1 bit - non valid pointer to massive
 * 2 bit - error in stack`s capacity or size
 * 3 bit - some movements changed information in stack buffer
 * 4 bit - canaries value had been changed
 */

ERRS StackDmp(Stack *st, long long errs_container) {
    if (st->dump_fptr == nullptr) {
        if (SECURITY_LVL == 0) {
            return NO_DUMP;
        } else {
            fprintf(stderr, "Error in dump opening, please check DumpInit function.\n");
            return NULL_POINTER;
        }
    }
    err_view(st, errs_container);

    if (get_bit(errs_container, 0) == 1) {
        fprintf(st->dump_fptr,
                "<p align=center><font color=%s>Stack has null pointer address, can`t dump it.</font></p>\n",
                colored(BAD));
        DumpClose(st);
        return NULL_POINTER;
    }
    fprintf(st->dump_fptr,
            "<p align=center>Stack dump called, pointer to stack is %p, size: %d, capacity: %d, size of one element: %d byte. "
            "Pointer to massive with data: %p</p>\n",
            st, st->size, st->capacity, st->element_sz, st->data);

    if (get_bit(errs_container, 1) == 1) {
        fprintf(st->dump_fptr,
                "<p align=center><font color=%s>Pointer to data element had been spoiled, can`t show data.</font></p>\n",
                colored(BAD));
        DumpClose(st);
        return NULL_POINTER;
    }


    return OK;
}

unsigned int hash_count(void* block_st, size_t length) {
    unsigned int hash = 0;
    for (int i = 0; i < length; i++) {
        hash += *((unsigned char*) block_st + i);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 17);

    return hash;
}

ERRS set_hash(Stack *st) {
    if (st == nullptr) {
        return NULL_POINTER;
    }

    st->hash_dat = hash_count(st->data, st->capacity * st->element_sz);
    st->hash_st = hash_count(st, STACK_SZ);
    return OK;
}

ERRS check_hash(Stack *st) {
    if (st->hash_dat != hash_count(st->data, st->capacity * st->element_sz)) {
        return BAD_ST_HASH;
    }
    else if (st->hash_st != hash_count(st, STACK_SZ)){
        return BAD_DAT_HASH;
    }

    return OK;
}