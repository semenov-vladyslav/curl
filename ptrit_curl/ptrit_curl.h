#pragma once

#include <stdint.h>

#define HASH_LENGTH_TRIT 243
#define STATE_LENGTH (3 * HASH_LENGTH_TRIT)

#if defined(PTRIT_NEON)
#include <arm_neon.h>
typedef uint64x2_t ptrit_s;
// -1 -> (0,0); 0 -> (1,0); +1 -> (1,1)
#define PTRIT_CVT_00_10_11

#else
#if defined(PTRIT_AVX512)
#if !defined(__AVX512F__)
#warning __AVX512F__ is not defined
#endif
#include <immintrin.h>
typedef __m512i ptrit_s;

#elif defined(PTRIT_AVX2)
#if !defined(__AVX2__)
#warning __AVX2__ is not defined
#endif
#include <immintrin.h>
typedef __m256i ptrit_s;

#elif defined(PTRIT_AVX)
#if !defined(__AVX__)
#warning __AVX__ is not defined
#endif
#include <immintrin.h>
typedef __m256d ptrit_s;

#elif defined(PTRIT_SSE2)
#if !defined(__SSE2__)
#warning __SSE2__ is not defined
#endif
#include <immintrin.h>
typedef __m128i ptrit_s;

#elif defined(PTRIT_SSE)
#if !defined(__SSE__)
#warning __SSE__ is not defined
#endif
#include <immintrin.h>
typedef __m128d ptrit_s;

#elif defined(PTRIT_64)
typedef uint64_t ptrit_s;

#else
#error No ptrit platform selected.

#endif
// -1 -> (1,0); 0 -> (1,1); +1 -> (0,1)
#define PTRIT_CVT_10_11_01

#endif

typedef struct {
  ptrit_s low;
  ptrit_s high;
} ptrit_t;

void ptrit_curl_sbox(ptrit_t *c, ptrit_t const *s);

#if defined(PTRIT_64)
void ptrit_curl_sbox_64(ptrit_t *const c, ptrit_t const *const s);
#endif

#if !defined(PTRIT_AVX512)
void ptrit_curl_sbox_dcurl(ptrit_t *c, ptrit_t const *s);
#endif
