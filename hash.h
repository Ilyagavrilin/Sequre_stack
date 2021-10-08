#ifndef FIRST_STACK_HASH_H
#define FIRST_STACK_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t murmur3_hash(const uint8_t* key, size_t len, uint32_t seed);

#endif //FIRST_STACK_HASH_H
