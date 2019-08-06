#include "ptrit_curl.h"

#include <string.h>
#include <assert.h>

#if defined(PTRIT_NEON)
#define ORN(x,y) vornq_u64(y,x)
#define XOR(x,y) veorq_u64(x,y)
#define AND(x,y) vandq_u64(x,y)
#define OR(x,y) vorrq_u64(x,y)
#define NOT(x) ORN(x,XOR(x,x))

#define XORORN(x,y,z) XOR(x,ORN(y,z))

#else
#if defined(PTRIT_AVX512F)
/*
  x y z x^(~y&z)  x^(y&z)
  0 0 0   0         0
  0 0 1   1         0
  0 1 0   0         0
  0 1 1   0         1
  1 0 0   1         1
  1 0 1   0         1
  1 1 0   1         1
  1 1 1   1         0
         D2        78
*/
#define XORANDN(x,y,z) _mm512_ternarylogic_epi64(x,y,z, 0xD2)
#define XORAND(x,y,z) _mm512_ternarylogic_epi64(x,y,z, 0x78)
#else

#if defined(PTRIT_AVX2)
#define ANDN(x,y) _mm256_andnot_si256(x,y)
#define AND(x,y) _mm256_and_si256(x,y)
#define XOR(x,y) _mm256_xor_si256(x,y)
#define OR(x,y) _mm256_or_si256(x,y)
#define NOT(x,y) _mm256_andnot_si256(x,_mm256_set_epi64x(-1ll, -1ll, -1ll, -1ll))

#elif defined(PTRIT_AVX)
#define ANDN(x,y) _mm256_andnot_pd(x,y)
#define AND(x,y) _mm256_and_pd(x,y)
#define XOR(x,y) _mm256_xor_pd(x,y)

#elif defined(PTRIT_SSE2)
#if defined(_MSC_VER) && _MSC_VER < 1900 /*&& !defined(_WIN64)*/
static __inline __m128i _mm_set_epi64x(__int64 _I1, __int64 _I0)
{
  __m128i i;
  i.m128i_i64[0] = _I0;
  i.m128i_i64[1] = _I1;
  return i;
}
#endif
#define ANDN(x,y) _mm_andnot_si128(x,y)
#define AND(x,y) _mm_and_si128(x,y)
#define XOR(x,y) _mm_xor_si128(x,y)
#define OR(x,y) _mm_or_si128(x,y)
#define NOT(x,y) _mm_andnot_si128(x,_mm_set_epi64x(-1ll, -1ll))

#elif defined(PTRIT_SSE)
#define ANDN(x,y) _mm_andnot_pd(x,y)
#define AND(x,y) _mm_and_pd(x,y)
#define XOR(x,y) _mm_xor_pd(x,y)

#else
#define ANDN(x,y) ((~(x)) & (y))
#define AND(x,y) ((x) & (y))
#define XOR(x,y) ((x) ^ (y))
#define OR(x,y) ((x) | (y))
#define NOT(x) (~(x))
#endif

#define XORANDN(x,y,z) XOR(x,ANDN(y,z))
#define XORAND(x,y,z) XOR(x,AND(y,z))
#endif
#endif

#define FORCE_INLINE
static FORCE_INLINE void curl_s2_andn(ptrit_t const *a, ptrit_t const *b, ptrit_t *c)
{
#if defined(PTRIT_CVT_00_10_11)
  // (Xor AH (Orn BL AL),Xor AL (Orn BH (Xor AH (Orn BL AL))))
  c->low = XORORN(a->high, b->low, a->low);
  c->high = XORORN(a->low, b->high, c->low);
#elif defined(PTRIT_CVT_10_11_01)
  // (Xor AH (Andn BL AL),Xor AL (And BH (Xor AH (Andn BL AL))))
  c->low = XORANDN(a->high, b->low, a->low);
  c->high = XORAND(a->low, b->high, c->low);
#else
#error No known ptrit cvt selected.
#endif
}

void ptrit_curl_sbox(ptrit_t *c, ptrit_t const *s)
{
  size_t i;

  // 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
  ptrit_t const *x = s + 0, *y = s + 364;
  curl_s2_andn(x, y, c++);
  x = s + 728;

  // 728 = 8*91
  for(i = 0; i < STATE_LENGTH / 8; ++i)
  {
    curl_s2_andn(y--, x, c++);
    curl_s2_andn(x--, y, c++);
    curl_s2_andn(y--, x, c++);
    curl_s2_andn(x--, y, c++);
    curl_s2_andn(y--, x, c++);
    curl_s2_andn(x--, y, c++);
    curl_s2_andn(y--, x, c++);
    curl_s2_andn(x--, y, c++);
  }
}

#define __INDEX_TABLE                                                                                                  \
  0, 364, 728, 363, 727, 362, 726, 361, 725, 360, 724, 359, 723, 358, 722, 357, 721, 356, 720, 355, 719, 354, 718,     \
      353, 717, 352, 716, 351, 715, 350, 714, 349, 713, 348, 712, 347, 711, 346, 710, 345, 709, 344, 708, 343, 707,    \
      342, 706, 341, 705, 340, 704, 339, 703, 338, 702, 337, 701, 336, 700, 335, 699, 334, 698, 333, 697, 332, 696,    \
      331, 695, 330, 694, 329, 693, 328, 692, 327, 691, 326, 690, 325, 689, 324, 688, 323, 687, 322, 686, 321, 685,    \
      320, 684, 319, 683, 318, 682, 317, 681, 316, 680, 315, 679, 314, 678, 313, 677, 312, 676, 311, 675, 310, 674,    \
      309, 673, 308, 672, 307, 671, 306, 670, 305, 669, 304, 668, 303, 667, 302, 666, 301, 665, 300, 664, 299, 663,    \
      298, 662, 297, 661, 296, 660, 295, 659, 294, 658, 293, 657, 292, 656, 291, 655, 290, 654, 289, 653, 288, 652,    \
      287, 651, 286, 650, 285, 649, 284, 648, 283, 647, 282, 646, 281, 645, 280, 644, 279, 643, 278, 642, 277, 641,    \
      276, 640, 275, 639, 274, 638, 273, 637, 272, 636, 271, 635, 270, 634, 269, 633, 268, 632, 267, 631, 266, 630,    \
      265, 629, 264, 628, 263, 627, 262, 626, 261, 625, 260, 624, 259, 623, 258, 622, 257, 621, 256, 620, 255, 619,    \
      254, 618, 253, 617, 252, 616, 251, 615, 250, 614, 249, 613, 248, 612, 247, 611, 246, 610, 245, 609, 244, 608,    \
      243, 607, 242, 606, 241, 605, 240, 604, 239, 603, 238, 602, 237, 601, 236, 600, 235, 599, 234, 598, 233, 597,    \
      232, 596, 231, 595, 230, 594, 229, 593, 228, 592, 227, 591, 226, 590, 225, 589, 224, 588, 223, 587, 222, 586,    \
      221, 585, 220, 584, 219, 583, 218, 582, 217, 581, 216, 580, 215, 579, 214, 578, 213, 577, 212, 576, 211, 575,    \
      210, 574, 209, 573, 208, 572, 207, 571, 206, 570, 205, 569, 204, 568, 203, 567, 202, 566, 201, 565, 200, 564,    \
      199, 563, 198, 562, 197, 561, 196, 560, 195, 559, 194, 558, 193, 557, 192, 556, 191, 555, 190, 554, 189, 553,    \
      188, 552, 187, 551, 186, 550, 185, 549, 184, 548, 183, 547, 182, 546, 181, 545, 180, 544, 179, 543, 178, 542,    \
      177, 541, 176, 540, 175, 539, 174, 538, 173, 537, 172, 536, 171, 535, 170, 534, 169, 533, 168, 532, 167, 531,    \
      166, 530, 165, 529, 164, 528, 163, 527, 162, 526, 161, 525, 160, 524, 159, 523, 158, 522, 157, 521, 156, 520,    \
      155, 519, 154, 518, 153, 517, 152, 516, 151, 515, 150, 514, 149, 513, 148, 512, 147, 511, 146, 510, 145, 509,    \
      144, 508, 143, 507, 142, 506, 141, 505, 140, 504, 139, 503, 138, 502, 137, 501, 136, 500, 135, 499, 134, 498,    \
      133, 497, 132, 496, 131, 495, 130, 494, 129, 493, 128, 492, 127, 491, 126, 490, 125, 489, 124, 488, 123, 487,    \
      122, 486, 121, 485, 120, 484, 119, 483, 118, 482, 117, 481, 116, 480, 115, 479, 114, 478, 113, 477, 112, 476,    \
      111, 475, 110, 474, 109, 473, 108, 472, 107, 471, 106, 470, 105, 469, 104, 468, 103, 467, 102, 466, 101, 465,    \
      100, 464, 99, 463, 98, 462, 97, 461, 96, 460, 95, 459, 94, 458, 93, 457, 92, 456, 91, 455, 90, 454, 89, 453, 88, \
      452, 87, 451, 86, 450, 85, 449, 84, 448, 83, 447, 82, 446, 81, 445, 80, 444, 79, 443, 78, 442, 77, 441, 76, 440, \
      75, 439, 74, 438, 73, 437, 72, 436, 71, 435, 70, 434, 69, 433, 68, 432, 67, 431, 66, 430, 65, 429, 64, 428, 63,  \
      427, 62, 426, 61, 425, 60, 424, 59, 423, 58, 422, 57, 421, 56, 420, 55, 419, 54, 418, 53, 417, 52, 416, 51, 415, \
      50, 414, 49, 413, 48, 412, 47, 411, 46, 410, 45, 409, 44, 408, 43, 407, 42, 406, 41, 405, 40, 404, 39, 403, 38,  \
      402, 37, 401, 36, 400, 35, 399, 34, 398, 33, 397, 32, 396, 31, 395, 30, 394, 29, 393, 28, 392, 27, 391, 26, 390, \
      25, 389, 24, 388, 23, 387, 22, 386, 21, 385, 20, 384, 19, 383, 18, 382, 17, 381, 16, 380, 15, 379, 14, 378, 13,  \
      377, 12, 376, 11, 375, 10, 374, 9, 373, 8, 372, 7, 371, 6, 370, 5, 369, 4, 368, 3, 367, 2, 366, 1, 365, 0

static size_t const CURL_INDEX[STATE_LENGTH + 1] = { __INDEX_TABLE };
// 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0

#if defined(PTRIT_64)
void ptrit_curl_sbox_64(ptrit_t *const c, ptrit_t const *const s)
{
  ptrit_s alpha, beta, delta;
  size_t i = 0;

  for(; i < STATE_LENGTH; ++i)
  {
    alpha = s[CURL_INDEX[i]].low;
    beta = s[CURL_INDEX[i]].high;
    delta = alpha & (s[CURL_INDEX[i + 1]].low ^ beta);

    c[i].low = ~delta;
    c[i].high = (alpha ^ s[CURL_INDEX[i + 1]].high) | delta;
  }
}
#endif

#if !defined(PTRIT_AVX512)
void ptrit_curl_sbox_dcurl(ptrit_t *c, ptrit_t const *s)
{
  size_t i;
  size_t t1, t2;
  ptrit_s alpha, beta, delta;
  for(i = 0; i < STATE_LENGTH; ++i)
  {
    t1 = CURL_INDEX[i];
    t2 = CURL_INDEX[i + 1];

    alpha = s[t1].low;
    beta = s[t1].high;
    delta = AND(alpha, XOR(s[t2].low, beta));
    /* alpha & (lfrom[t2] ^ beta) */
    c[i].low = NOT(delta); /* ~delta */
    c[i].high = OR(XOR(alpha, s[t2].high),
      delta); /* (alpha ^ hfrom[t2]) | delta */
  }
}
#endif
