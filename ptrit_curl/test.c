#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "ptrit_curl.h"

#if !defined(BENCH_SBOX_COUNT)
#define BENCH_SBOX_COUNT (330000 / (PTRIT_SIZE / 64))
#endif
#if !defined(BENCH_TX_COUNT)
#define BENCH_TX_COUNT (BENCH_SBOX_COUNT / 33)
#endif

#define NO_BENCH_SBOX

#ifndef NO_BENCH_SBOX
#if defined(PCURL_SBOX_OPT)
void pcurl_sbox_0(ptrit_t *a, ptrit_t *b, ptrit_t *c);
#else
void pcurl_sbox(ptrit_t *c, ptrit_t const *s);
#endif
#endif

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

#define DATA                                                                                                     \
  -1, 1, -1, -1, 1, -1, 1, 1, 0, -1, 0, 0, 1, 0, 1, 0, 0, 0, -1, -1, -1, -1, 0, 0, -1, 0, 0, 1, 0, 0, -1, 0, 0, 1, -1, \
  -1, 1, -1, 1, -1, -1, 1, 0, 1, 0, 0, 0, 1, -1, 0, -1, 1, -1, -1, 0, 0, 0, -1, 0, 0, 1, -1, -1, 0, 0, 0, -1, 0,   \
  0, 0, -1, -1, 0, 1, 1, -1, 1, 1, 1, 1, -1, 0, -1, 0, -1, 0, -1, 0, -1, -1, -1, -1, 0, 1, -1, 0, -1, -1, 0, 0, 0, \
  0, 0, 1, 1, 0, 1, -1, 0, -1, -1, -1, 0, 0, 1, 0, -1, -1, -1, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, 1, 1, -1, -1,   \
  1, 1, -1, 1, -1, 1, 0, -1, 1, -1, -1, -1, 0, 1, 1, 0, -1, 0, 1, 0, 0, 1, 1, 0, 0, -1, -1, 1, 0, 0, 0, 0, -1, 1,  \
  0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 0, -1, 1, 1, -1, 0, 0, 1, -1, 0, 1, 0, -1, 1, -1, 0, 0, 1, -1, -1, -1, 0, 1, 0,  \
  -1, -1, 0, 1, 0, 0, 0, 1, -1, 1, -1, 0, 1, -1, -1, 0, 0, 0, -1, -1, 1, 1, 0, 1, -1, 0, 0, 0, -1, 0, -1, 0, -1,   \
  -1, -1, -1, 0, 1, -1, -1, 0, 1
#define PCURL_27_HASH                                                                                                  \
  -1, -1, -1, -1, 0, 0, 1, 1, -1, 1, 1, 0, -1, 1, 0, 1, 0, 0, 1, 0, -1, 1, 1, -1, -1, -1, 0, 1, 0, 1, -1, -1, 1, -1,   \
  -1, -1, -1, 1, 1, 1, 1, -1, 1, 1, 1, -1, 0, 1, -1, 1, 0, 0, 1, -1, 1, -1, 1, 0, 1, 0, 0, 1, -1, 1, 1, -1, 0, 0,  \
  1, 1, -1, 0, 1, 0, -1, 0, 0, 1, -1, -1, -1, 0, 0, -1, 1, 0, 0, -1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, -1, 1, 0,    \
  -1, 1, 0, 1, 1, 0, 0, -1, 1, -1, 1, 0, -1, 0, 1, 0, 1, -1, 1, -1, 0, 1, 0, 1, 1, 1, -1, 0, 1, -1, 0, 0, 0, 1, 0, \
  -1, 0, -1, 0, -1, -1, 1, -1, 1, 1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 0, 0, -1, 0, 0, -1, 0, -1, -1, 0, 0, -1, -1, \
  1, 1, -1, -1, -1, 0, -1, 0, -1, -1, 1, -1, -1, -1, -1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, -1, 1, 0, 1, -1, -1, -1,    \
  -1, 1, 0, 0, -1, 1, 1, 1, -1, 1, 0, -1, 0, 1, -1, 1, 1, 1, 0, 1, 1, 0, -1, 0, 1, 1, -1, 0, -1, 0, 1, 0, 0, 1, 1, \
  1, -1, 0, 1, -1, 0

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

void test_pcurl_s2()
{
  int r;
  ptrit_t a, b, c;
  for(r = 0; r < 16; ++r)
  {
    a.low = (r & (1 << 0)) ? -1 : 0;
    a.high = (r & (1 << 1)) ? -1 : 0;
    b.low = (r & (1 << 2)) ? -1 : 0;
    b.high = (r & (1 << 3)) ? -1 : 0;
    pcurl_s2(&a, &b, &c);
    //if((a.low || a.high) && (b.low || b.high) && (c.low || c.high))
    printf("a=(%d,%d) b=(%d,%d) c=(%d,%d)\n"
      , (int)(a.low ? 1 : 0), (int)(a.high ? 1 : 0)
      , (int)(b.low ? 1 : 0), (int)(b.high ? 1 : 0)
      , (int)(c.low ? 1 : 0), (int)(c.high ? 1 : 0)
    );
  }
  r = 0;
}
#endif

void test_curl()
{
  trit_te1_t const data[] = { DATA };
  trit_te1_t const hash[] = { PCURL_27_HASH };
  ptrit_t pdata[sizeof(data)];
  ptrit_t ph2[sizeof(hash)];
  trit_te1_t h2[sizeof(hash)];

  pcurl_t curl;
  printf("test_curl 0\n");
  pcurl_init(&curl, 27);
  printf("test_curl 00\n");
  memset(pdata, 0, sizeof(pdata));
  printf("test_curl 1\n");
  trits_te1_to_tep(pdata, 0, data, sizeof(data));
  printf("test_curl 2\n");
  trits_te1_to_tep(pdata, PTRIT_SIZE - 4, data, sizeof(data));
  printf("test_curl 3\n");
  pcurl_absorb(&curl, pdata, sizeof(data));
  printf("test_curl 4\n");
  //pcurl_squeeze(&curl, ph2, sizeof(hash));
  pcurl_get_hash(&curl, ph2);
  printf("test_curl 5\n");
  ptrits_print(RATE, ph2);

  {
    int r = 1;
    trits_tep_to_te1(h2, ph2, 0, sizeof(hash));
    printf("test_curl 6\n");
    r = (0 == memcmp(hash, h2, sizeof(hash)));
    printf("test_curl0 %s\n", r ? "ok" : "failed");
    assert(r);
    trits_tep_to_te1(h2, ph2, PTRIT_SIZE - 4, sizeof(hash));
    printf("test_curl 7\n");
    r = (0 == memcmp(hash, h2, sizeof(hash)));
    printf("test_curl1 %s\n", r ? "ok" : "failed");
    assert(r);
  }
}

#ifndef NO_BENCH_SBOX
void test_sbox()
{
  int r = 1;
  uint64_t rnd = 0x88534582;

  ptrit_t s[STATE_SIZE]
    , c[STATE_SIZE]
    , c2[STATE_SIZE]
    ;
  size_t i;
  for(i = 0; i < STATE_SIZE; ++i)
  {
    s[i] = rng_ptrit(&rnd);
  }

  pcurl_sbox(c, s);

#if defined(PCURL_SBOX_INDEX)
#if defined(PTRIT_64)
  pcurl_sbox_64(c2, s);
  r = memcmp(c, c2, sizeof(c));
  printf("pcurl_sbox_64 test %s\n", r ? "failed" : "ok");
#endif

#if !defined(PTRIT_AVX512)
  pcurl_sbox_dcurl(c2, s);
  r = memcmp(c, c2, sizeof(c));
  printf("pcurl_sbox_dcurl test %s\n", r ? "failed" : "ok");
#endif
#endif
}

void bench_pcurl_sbox()
{
  ptrit_t s[STATE_SIZE]
    , c[STATE_SIZE]
    ;
  size_t i = BENCH_SBOX_COUNT;
  clock_t runtime = clock();
  for(; i--;)
    pcurl_sbox(c, s);
  runtime = clock() - runtime;
  printf("pcurl_sbox\t: %d\n", (int)runtime);
}

#if defined(PCURL_SBOX_INDEX)
#if defined(PTRIT_64)
void bench_pcurl_sbox_64()
{
  ptrit_t s[STATE_SIZE]
    , c[STATE_SIZE]
    ;
  size_t i = BENCH_SBOX_COUNT;
  clock_t runtime = clock();
  for(; i--;)
    pcurl_sbox_64(c, s);
  runtime = clock() - runtime;
  printf("pcurl_sbox_64  \t: %d\n", (int)runtime);
}
#endif

#if !defined(PTRIT_AVX512)
void bench_pcurl_sbox_dcurl()
{
  ptrit_t s[STATE_SIZE]
    , c[STATE_SIZE]
    ;
  size_t i = BENCH_SBOX_COUNT;
  clock_t runtime = clock();
  for(; i--;)
    pcurl_sbox_dcurl(c, s);
  runtime = clock() - runtime;
  printf("pcurl_sbox_dcurl\t: %d\n", (int)runtime);
}
#endif
#endif

void bench_sbox()
{
  bench_pcurl_sbox();
#if defined(PCURL_SBOX_INDEX)
#if defined(PTRIT_64)
  bench_pcurl_sbox_64();
#endif
#if !defined(PTRIT_AVX512)
  bench_pcurl_sbox_dcurl();
#endif
#endif
}
#endif

void bench_hash()
{
  ptrit_t tx[8019];
  ptrit_t hash[RATE];
  pcurl_t c;

  memset(tx, -1, sizeof(tx));
  
  size_t i = BENCH_TX_COUNT;
  clock_t runtime = clock();
  for(; i--;)
    pcurl_hash_data(&c, tx, 8019, hash);
  runtime = clock() - runtime;
  printf("pcurl_hash_data\t: %d\n", (int)runtime);
}

int main()
{
  test_curl();
  return 0;
#ifndef NO_BENCH_SBOX
  bench_sbox();
  bench_sbox();
  bench_sbox();
#endif
  bench_hash();
  bench_hash();
  bench_hash();

  return 0;
}
