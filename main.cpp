
#include <ostream>
#include "stack_funcs.h"

int main() {
    Stack st = {};
    long long errs_container = 0;
    StackCtor(&st, 0, sizeof(int));
    DumpClose(&st);
    int a = 5;
    StackPush(&st, &a, &errs_container);
    int b = 0;
    StackPop(&st, &b, &errs_container);
    printf("%p\n", &b);
    StackDtor(&st);


    return 0;
}
