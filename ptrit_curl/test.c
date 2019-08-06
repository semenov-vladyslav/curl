#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "ptrit_curl.h"

uint64_t rng_step(uint64_t *rnd)
{
  uint64_t d = (*rnd << 13) + 0x12345678 - (*rnd >> 7) * 7736287445;
  *rnd = ((*rnd + 0xfeedbad) * 1177531961) ^ 0xdeadbeef;
  return d;
}

ptrit_t rng_ptrit(uint64_t *rnd)
{
  ptrit_t t;
  uint64_t *low = (uint64_t *)&t.low;
  uint64_t *high = (uint64_t *)&t.high;
  uint64_t s;
  size_t i = sizeof(t.low) / sizeof(uint64_t);
  for(; i--; ++low, ++high)
  {
    *low = rng_step(rnd);
    *high = rng_step(rnd);
    s = *low | *high;
    *low |= ~s;
  }
  return t;
}

#if 0
void test_curl_s2_trit()
{
  int a, b, c;
  for(b = -1; b <= 1; ++b)
  for(a = -1; a <= 1; ++a)
  {
    c = curl_s2_trit(a, b);
    printf("a=%d b=%d c=%d\n", a, b, c);
  }
  c = 0;
}

void test_curl_s2()
{
  int r;
  ptrit_t a, b, c;
  for(r = 0; r < 16; ++r)
  {
    a.low = (r & (1 << 0)) ? -1 : 0;
    a.high = (r & (1 << 1)) ? -1 : 0;
    b.low = (r & (1 << 2)) ? -1 : 0;
    b.high = (r & (1 << 3)) ? -1 : 0;
    c = curl_s2(a, b);
    if((a.low || a.high) && (b.low || b.high) && (c.low || c.high))
    printf("a=(%d,%d) b=(%d,%d) c=(%d,%d)\n"
      , (int)(a.low ? 1 : 0), (int)(a.high ? 1 : 0)
      , (int)(b.low ? 1 : 0), (int)(b.high ? 1 : 0)
      , (int)(c.low ? 1 : 0), (int)(c.high ? 1 : 0)
    );
  }
  r = 0;
}

void test_curl_s2_andn()
{
  int r;
  ptrit_t a, b, c;
  for(r = 0; r < 16; ++r)
  {
    a.low = (r & (1 << 0)) ? -1 : 0;
    a.high = (r & (1 << 1)) ? -1 : 0;
    b.low = (r & (1 << 2)) ? -1 : 0;
    b.high = (r & (1 << 3)) ? -1 : 0;
    c = curl_s2_andn(a, b);
    if((a.low || a.high) && (b.low || b.high) && (c.low || c.high))
    printf("a=(%d,%d) b=(%d,%d) c=(%d,%d)\n"
      , (int)(a.low ? 1 : 0), (int)(a.high ? 1 : 0)
      , (int)(b.low ? 1 : 0), (int)(b.high ? 1 : 0)
      , (int)(c.low ? 1 : 0), (int)(c.high ? 1 : 0)
    );
  }
  r = 0;
}
#endif

void test_sbox()
{
  int r = 1;
  uint64_t rnd = 0x88534582;

  ptrit_t s[STATE_LENGTH]
    , c[STATE_LENGTH]
    , c2[STATE_LENGTH]
    ;
  size_t i;
  for(i = 0; i < STATE_LENGTH; ++i)
  {
    s[i] = rng_ptrit(&rnd);
  }

  ptrit_curl_sbox(c, s);

#if defined(PTRIT_64)
  ptrit_curl_sbox_64(c2, s);
  r = memcmp(c, c2, sizeof(c));
  printf("ptrit_curl_sbox_64 test %s\n", r ? "failed" : "ok");
#endif

#if !defined(PTRIT_AVX512)
  ptrit_curl_sbox_dcurl(c2, s);
  r = memcmp(c, c2, sizeof(c));
  printf("ptrit_curl_sbox_dcurl test %s\n", r ? "failed" : "ok");
#endif
}

#define BENCH_SBOX_COUNT (330000)

void bench_ptrit_curl_sbox()
{
  ptrit_t s[STATE_LENGTH]
    , c[STATE_LENGTH]
    ;
  size_t i = BENCH_SBOX_COUNT / (sizeof(ptrit_s) / sizeof(uint64_t));
  clock_t runtime = clock();
  for(; i--;)
    ptrit_curl_sbox(c, s);
  runtime = clock() - runtime;
  printf("ptrit_curl_sbox time = %d\n", (int)runtime);
}

#if defined(PTRIT_64)
void bench_ptrit_curl_sbox_64()
{
  ptrit_t s[STATE_LENGTH]
    , c[STATE_LENGTH]
    ;
  size_t i = BENCH_SBOX_COUNT / (sizeof(ptrit_s) / sizeof(uint64_t));
  clock_t runtime = clock();
  for(; i--;)
    ptrit_curl_sbox_64(c, s);
  runtime = clock() - runtime;
  printf("ptrit_curl_sbox_64   time = %d\n", (int)runtime);
}
#endif

#if !defined(PTRIT_AVX512)
void bench_ptrit_curl_sbox_dcurl()
{
  ptrit_t s[STATE_LENGTH]
    , c[STATE_LENGTH]
    ;
  size_t i = BENCH_SBOX_COUNT / (sizeof(ptrit_s) / sizeof(uint64_t));
  clock_t runtime = clock();
  for(; i--;)
    ptrit_curl_sbox_dcurl(c, s);
  runtime = clock() - runtime;
  printf("ptrit_curl_sbox_dcurl time = %d\n", (int)runtime);
}
#endif

void bench_sbox()
{
  bench_ptrit_curl_sbox();
#if defined(PTRIT_64)
  bench_ptrit_curl_sbox_64();
#endif
#if !defined(PTRIT_AVX512)
  bench_ptrit_curl_sbox_dcurl();
#endif
}

int main()
{
  test_sbox();
  bench_sbox();
  bench_sbox();
  bench_sbox();
  bench_sbox();
  bench_sbox();
  bench_sbox();

  //test_curl_s2();
  //test_curl_s2_trit();
  //test_curl_s2_andn();
  //xx();
  return 0;
}
