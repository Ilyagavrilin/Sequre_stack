
#include "stack_funcs.h"

int main() {
    Stack st = {};
    DumpInit(&st);
    long long a = 0ll;
    set_bit(3, &a, sizeof(long long));
    set_bit(2, &a, sizeof(long long));
    set_bit(4, &a, sizeof(long long));
    StackDmp(&st, a);
    DumpClose(&st);
    return 0;
}
