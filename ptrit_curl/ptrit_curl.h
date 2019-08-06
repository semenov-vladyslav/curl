#pragma once

#include <stdint.h>
#include <immintrin.h>

#define HASH_LENGTH_TRIT 243
#define STATE_LENGTH (3 * HASH_LENGTH_TRIT)

#if defined(__AVX512F__)
#define PTRIT_AVX512
typedef __m512i ptrit_s;
#elif defined(__AVX2__)
#define PTRIT_AVX2
typedef __m256i ptrit_s;
#elif defined(__AVX__)
#define PTRIT_AVX
typedef __m256d ptrit_s;
#elif defined(__SSE2__)
#define PTRIT_SSE2
typedef __m128i ptrit_s;
#elif defined(__SSE__)
#define PTRIT_SSE
typedef __m128d ptrit_s;
#else
#define PTRIT_64
typedef uint64_t ptrit_s;
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
