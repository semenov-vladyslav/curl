#pragma once

#include <stdint.h>

#define RATE 243
#define STATE_SIZE (3 * RATE)
#define HASH_LENGTH RATE

#if defined(PTRIT_64)
typedef uint64_t ptrit_s;
#define PTRIT_SIZE 64
#if !defined(PTRIT_CVT_ORN) && !defined(PTRIT_CVT_ANDN)
#define PTRIT_CVT_ANDN
#endif

#elif defined(PTRIT_NEON)
#include <arm_neon.h>
typedef uint64x2_t ptrit_s;
#define PTRIT_SIZE 128
#if defined(PTRIT_CVT_ANDN)
#error ARM NEON curl impl must use PTRIT_CVT_ORN
#endif
#if !defined(PTRIT_CVT_ORN)
// -1 -> (0,0); 0 -> (1,0); +1 -> (1,1)
#define PTRIT_CVT_ORN
#endif

#else
#if defined(PTRIT_AVX512)
#if !defined(__AVX512F__)
#warning __AVX512F__ is not defined
#endif
#include <immintrin.h>
typedef __m512i ptrit_s;
#define PTRIT_SIZE 512

#elif defined(PTRIT_AVX2)
#if !defined(__AVX2__)
#warning __AVX2__ is not defined
#endif
#include <immintrin.h>
typedef __m256i ptrit_s;
#define PTRIT_SIZE 256

#elif defined(PTRIT_AVX)
#if !defined(__AVX__)
#warning __AVX__ is not defined
#endif
#include <immintrin.h>
typedef __m256d ptrit_s;
#define PTRIT_SIZE 256

#elif defined(PTRIT_SSE2)
#if !defined(__SSE2__)
#warning __SSE2__ is not defined
#endif
#include <immintrin.h>
typedef __m128i ptrit_s;
#define PTRIT_SIZE 128

#elif defined(PTRIT_SSE)
#if !defined(__SSE__)
#warning __SSE__ is not defined
#endif
#include <immintrin.h>
typedef __m128d ptrit_s;
#define PTRIT_SIZE 128

#else
#error No ptrit platform selected.

#endif

#if defined(PTRIT_CVT_ORN)
#error Intel intrinsics curl impl must use PTRIT_CVT_ANDN
#endif
#if !defined(PTRIT_CVT_ANDN)
// -1 -> (1,0); 0 -> (1,1); +1 -> (0,1)
#define PTRIT_CVT_ANDN
#endif

#endif

typedef struct {
  ptrit_s low;
  ptrit_s high;
} ptrit_t;

typedef int8_t trit_te1_t;

void trits_te1_to_tep(
  ptrit_t *dst, size_t idx, // dst must be zero, idx < PTRIT_SIZE
  trit_te1_t const *src,
  size_t n);

void trits_tep_to_te1(
  trit_te1_t *dst,
  ptrit_t const *src, size_t idx, // idx < PTRIT_SIZE
  size_t n);

void pcurl_sbox(ptrit_t *c, ptrit_t const *s);

#define PCURL_MEM_SHORT

typedef struct
{
#if defined(PCURL_SBOX_MEMSHORT)
  ptrit_t a[(STATE_SIZE + 1) / 2];
  ptrit_t b[(STATE_SIZE + 1) / 2];
  ptrit_t c[(STATE_SIZE + 1) / 2];
#else
  ptrit_t a[STATE_SIZE];
  ptrit_t c[STATE_SIZE];
#endif
  size_t round_count;
} pcurl_t;

void pcurl_init(pcurl_t *ctx, size_t round_count);
void pcurl_absorb(pcurl_t *ctx, ptrit_t const* ptrits, size_t length);
void pcurl_squeeze(pcurl_t *ctx, ptrit_t* ptrits, size_t length);
void pcurl_get_hash(pcurl_t *ctx, ptrit_t* hash);
void pcurl_transform(pcurl_t *ctx);
void pcurl_reset(pcurl_t *ctx);


#if defined(PTRIT_64)
void pcurl_sbox_64(ptrit_t *const c, ptrit_t const *const s);
#endif

#if !defined(PTRIT_AVX512)
void pcurl_sbox_dcurl(ptrit_t *c, ptrit_t const *s);
#endif
