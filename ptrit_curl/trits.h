#pragma once

#include <stdint.h>

typedef int8_t trit_te1_t; // -1, 0, 1
typedef char trit_te3_t; // '9':0,'A':1,..,'Z':-1
typedef uint8_t trit_te4_t; // 2 bits per trit; 4 trits packed into 8 bits
typedef uint8_t trit_te5_t; // ints mod 3^5
typedef uint16_t trit_te9_t; // 3 trytes per 16 bits sa in bee

typedef int err_t; // 0 - ok, /=0 - error
