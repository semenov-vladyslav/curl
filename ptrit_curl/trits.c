#include "trits.h"

#include <assert.h>
#include <memory.h>

#if 0
void trits_teM_to_teN( //M,N={1,3,4,5,9}
  trit_teN_t *dst, size_t dst_off, // dst_off < N
  trit_teM_t const *src, size_t src_off, // src_off < M
  size_t n);
err_t trits_teM_to_teN_s( //M,N={1,3,4,5,9}
  trit_teN_t *dst, size_t dst_off, size_t dst_size, // dst_off < dst_size
  trit_teM_t const *src, size_t src_off, size_t src_size, // src_off < src_size
  size_t n); // n <= dst_size - dst_off && n <= src_size - src_off
#endif

void trits_te1_to_te1(
  trit_te1_t *dst, size_t dst_off,
  trit_te1_t const *src, size_t src_off,
  size_t n) {
  assert(0 == dst_off);
  assert(0 == src_off);
  memcpy(dst, src, n);
}

err_t trits_te1_to_te1_s(
  trit_te1_t *dst, size_t dst_off, size_t dst_size,
  trit_te1_t const *src, size_t src_off, size_t src_size,
  size_t n) { // n <= dst_size - dst_off && n <= src_size - src_off
  if (!(1
      && dst_off < dst_size
      && src_off < src_size
      && n <= dst_size
      && n <= src_size
    )) return 1;

  trits_te1_to_te1(dst + dst_off, 0, src + src_off, 0, n);
  return 0;
}

void trits_te1_to_te3(
  trit_te1_t *dst, size_t dst_off,
  trit_te3_t const *src, size_t src_off,
  size_t n) {
  assert(0 == dst_off);
  assert(0 == src_off);
  memcpy(dst, src, n);
}

err_t trits_te1_to_te3_s(
  trit_te1_t *dst, size_t dst_off, size_t dst_size,
  trit_te3_t const *src, size_t src_off, size_t src_size,
  size_t n) { // n <= dst_size - dst_off && n <= src_size - src_off
  if (!(1
      && dst_off < dst_size
      && src_off < src_size
      && n <= dst_size
      && n <= src_size
    )) return 1;

  trits_te1_to_te1(dst + dst_off, 0, src + src_off, 0, n);
  return 0;
}

void trits_te3_to_te5(
  trit_te3_t *dst, size_t dst_off,
  trit_te5_t const *src, size_t src_off,
  size_t n)
{
}

