# PCurl

Fast reference implementation of vectorized IOTA Curl transform function; tests and benchmarks.

# Design overview

PCurl runs `n` Curl instances at once following SIMD idea. A trit is represented with 2 bits `(low,high)`; there are `4! = 24` such representations.
`n` trits thus can be packed into two `word`s: one containing `n` `low` bits and the other -- `n` `high` bits, provided `n` bits fit `word`.
A pair of `word`s is called `dword`. PCurl `state` consists of 743 `dwords`. The building block for a Sponge construction is `transform : state -> state` mapping which
applies simpler transform `pcurl_sbox : state -> state` `r` times. The following property can be stated regarding `i`-th output state trit of `pcurl_sbox`:

```
pcurl_sbox(s)[i] == pcurl_s2(s[a(i)], s[b(i)]),
```

where `pcurl_s2 : dword x dword -> dword` is a building block of PCurl that mixes two input trits into one output trit. `a, b : Z_743 -> Z_743` are just fixed
permutations that determine dependencies between input and output trits. The circuit evaluating `pcurl_s2` depends on trit representation and on availability
of corresponding processor instructions.

# Optimizations

There are a number of optimization parameters that can be selected at compile time. They are compared against the "basic" PCurl implementation in iotaledger/entangled.

## `PTRIT_CVT`
Trit representation.

- `PTRIT_CVT_ANDN`: `-1 -> (1,0)`, `0 -> (1,1)`, `+1 -> (0,1)`; optimizes `pcurl_s2` for `andn` instruction; used in the "basic" PCurl impl.

- `PTRIT_CVT_ORN`: `-1 -> (0,0)`, `0 -> (0,1)`, `+1 -> (1,1)`; optimizes `pcurl_s2` for `orn` instruction.

## `PTRIT_PLATFORM`
A straight-forward idea is to SIMD intrinsics to extend capacity of `word` and use available instructions for circuit evaluation.

- `PTRIT_NEON`: 128-bit NEON intrinsics on ARMv7-a targets; enforces the use of `PTRIT_CVT_ORN`.

- `PTRIT_AVX512`: 512-bit AVX512F intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_AVX2`: 256-bit AVX2 intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_SSE2`: 128-bit SSE2 intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_64`: `word` is just 64-bit `uint64_t` type and standard C syntax is used to express `pcurl_s2` circuit.

## `PTRIT_S2_CIRCUIT`
Enables the use of certain `pcurl_s2` circuit.

- `PTRIT_S2_CIRCUIT4`: optimized 4-instruction circuits found with `circuit-solver`.
  With `PTRIT_CVT_ANDN` -- `(Xor AH (Andn BL AL),Xor AL (And BH (Xor AH (Andn BL AL))))`.
  With `PTRIT_CVT_OR` -- `(Xor AH (Orn BL AL),Xor AL (Orn BH (Xor AH (Orn BL AL))))`.

- `PTRIT_S2_CIRCUIT5`: 5-instruction circuit used in the "basic" PCurl impl.

## `PCURL_S2_ARGS`
Determines the way arguments `s[a(i)]` and `s[b(i)]` to `pcurl_s2` are selected, ie. the way `a` and `b` permutations are implemented.

- `PCURL_S2_ARGS_PTR`: through pointer arithmetic.

- `PCURL_S2_ARGS_LUT`: through look-up table as in the "basic" PCurl impl.

## `PCURL_SBOX_UNWIND`
Unwind factor for `pcurl_sbox` internal loop, ie. the number of call to `pcurl_s2` per loop. Not all unwind factors are allowed with certain configurations.

- `PCURL_SBOX_UNWIND_1`: 1, ie. do not unwind.

- `PCURL_SBOX_UNWIND_2`: 2.

- `PCURL_SBOX_UNWIND_4`: 4.

- `PCURL_SBOX_UNWIND_8`: 8.

## `PCURL_STATE`
`pcurl_sbox` requires additional memory as output trits are highly interconnected with input.

- `PCURL_STATE_SHORT`: use roughly half of state as additional. It is achieved through different `pcurl_sbox`es as they access memory in different fashion.

- `PCURL_STATE_DOUBLE`: trivially use a full state as additional as in the "basic" PCurl impl.

# Build and Run

See [bench_arm.sh](bench_arm.sh), [bench_x64_msvc.bat](bench_x64_msvc.bat), and [bench_x64_gcc.bat](bench_x64_gcc.bat) for examples. You might need to adjust your `PATH` environment variable.

# Benchmark results

Time is measured using `clock` function, benchmark results have quite significant variance due to platform specifics (mobile PC, noisy environment).
Time presented is "average" of several benchmark runs.

Short config has the following format:

```
<compiler>_<platform>_<cvt>_<circuit>_<s2_args>_<unwind>_<state>
```

where
- `<compiler>`: `gcc`, `clang`, or `msvc`; version omitted.
- `<platform>`: `neon`=`PTRIT_NEON`, `avx512`=`PTRIT_AVX512`, `avx2`=`PTRIT_AVX2`, `sse2`=`PTRIT_SSE2`, `64`=`PTRIT_64`.
- `<cvt>`: `andn`=`PTRIT_CVT_ANDN`, `orn`=`PTRIT_CVT_ORN`.
- `<circuit>`: `c4`=`PCURL_S2_CIRCUIT4`, `c5`=`PCURL_S2_CIRCUIT5`.
- `<s2_args>`: `ptr`=`PCURL_S2_ARGS_PTR`, `lut`=`PCURL_S2_ARGS_LUT`.
- `<unwind>`: `uX`=`PCURL_SBOX_UNWIND_X`.
- `<state>`: `ss`=`PCURL_STATE_SHORT`, `sd`=`PCURL_STATE_DOUBLE`.

## Laptop HP ProBook

Environment: Intel Core i7 4510U, Windows 8.1, msvc-14.1, gcc-8.0.1.
AVX512 implementation was tested with Intel sde-8.35.0.

*Speedup*: 2.71x.

| Short config                   | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------: | ------------: | -------: |
| `gcc_avx512_andn_c4_ptr_u4_ss` |   575 tx/s |   512 x 10 tx |  8897 ms |
|                                |   579 tx/s |   512 x 10 tx |  8836 ms |
|                                |   572 tx/s |   512 x 10 tx |  8940 ms |
| `gcc_sse2_andn_c4_ptr_u4_ss`   | 55541 tx/s | 128 x 5000 tx | 11523 ms |
|                                | 48724 tx/s | 128 x 5000 tx | 13135 ms |
|                                | 46299 tx/s | 128 x 5000 tx | 13823 ms |
| `msvc_sse2_andn_c4_ptr_u4_ss`  | 48799 tx/s | 128 x 5000 tx | 13115 ms |
|                                | 48167 tx/s | 128 x 5000 tx | 13287 ms |
|                                | 46879 tx/s | 128 x 5000 tx | 13652 ms |
| `msvc_avx2_andn_c4_ptr_u4_ss`  | 47672 tx/s | 256 x 2500 tx | 13425 ms |
|                                | 47693 tx/s | 256 x 2500 tx | 13419 ms |
|                                | 47038 tx/s | 256 x 2500 tx | 13606 ms |
| `gcc_avx2_andn_c4_ptr_u4_ss`   | 47107 tx/s | 256 x 2500 tx | 13586 ms |
|                                | 46931 tx/s | 256 x 2500 tx | 13637 ms |
|                                | 47368 tx/s | 256 x 2500 tx | 13511 ms |
| `msvc_avx2_andn_c4_ptr_u4_sd`  | 45133 tx/s | 256 x 2500 tx | 14180 ms |
|                                | 45045 tx/s | 256 x 2500 tx | 14208 ms |
|                                | 45169 tx/s | 256 x 2500 tx | 14169 ms |
| `msvc_avx2_andn_c4_ptr_u2_sd`  | 43354 tx/s | 256 x 2500 tx | 14762 ms |
|                                | 43950 tx/s | 256 x 2500 tx | 14562 ms |
|                                | 43281 tx/s | 256 x 2500 tx | 14787 ms |
| `gcc_avx2_andn_c4_ptr_u2_sd`   | 42780 tx/s | 256 x 2500 tx | 14960 ms |
|                                | 42108 tx/s | 256 x 2500 tx | 15199 ms |
|                                | 37941 tx/s | 256 x 2500 tx | 16868 ms |
| `gcc_avx2_andn_c4_ptr_u8_sd`   | 41674 tx/s | 256 x 2500 tx | 15357 ms |
|                                | 40145 tx/s | 256 x 2500 tx | 15942 ms |
|                                | 36527 tx/s | 256 x 2500 tx | 17521 ms |
| `gcc_avx2_andn_c4_ptr_u4_sd`   | 39438 tx/s | 256 x 2500 tx | 16228 ms |
|                                | 39885 tx/s | 256 x 2500 tx | 16046 ms |
|                                | 40231 tx/s | 256 x 2500 tx | 15908 ms |
| `msvc_64_andn_c4_ptr_u4_ss`    | 24781 tx/s | 64 x 10000 tx | 25826 ms |
|                                | 29723 tx/s | 64 x 10000 tx | 21532 ms |
|                                | 29684 tx/s | 64 x 10000 tx | 21560 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     | 28567 tx/s | 64 x 10000 tx | 22403 ms |
|                                | 28731 tx/s | 64 x 10000 tx | 22275 ms |
|                                | 28519 tx/s | 64 x 10000 tx | 22441 ms |
| `gcc_64_andn_c5_lut_u1_sd`     | 18489 tx/s | 64 x 10000 tx | 34615 ms |
|                                | 18003 tx/s | 64 x 10000 tx | 35549 ms |
|                                | 16341 tx/s | 64 x 10000 tx | 39164 ms |
| `msvc_64_andn_c5_lut_u1_sd`    | 16291 tx/s | 64 x 10000 tx | 39284 ms |
|                                | 16296 tx/s | 64 x 10000 tx | 39272 ms |
|                                | 16044 tx/s | 64 x 10000 tx | 39890 ms |

## Raspberry Pi 3

Environment: ARMv7l, raspbian (4.4.13-v7+), gcc-6.1.0.

*Speedup*: 1.82x.

| Short config                   | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------- | ------------- | -------- |
| `gcc_64_andn_c4_ptr_u8_sd`     | 10245 tx/s |  64 x 1000 tx |  6246 ms |
|                                | 10299 tx/s |  64 x 1000 tx |  6213 ms |
|                                | 10300 tx/s |  64 x 1000 tx |  6213 ms |
| `gcc_neon_orn_c4_ptr_u4_ss`    | 10020 tx/s |  128 x 500 tx |  6386 ms |
|                                | 10090 tx/s |  128 x 500 tx |  6342 ms |
|                                | 10105 tx/s |  128 x 500 tx |  6332 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     |  9925 tx/s |  64 x 1000 tx |  6448 ms |
|                                |  9937 tx/s |  64 x 1000 tx |  6440 ms |
|                                |  9848 tx/s |  64 x 1000 tx |  6498 ms |
| `gcc_64_orn_c4_ptr_u4_ss`      |  8167 tx/s |  64 x 1000 tx |  7836 ms |
|                                |  8156 tx/s |  64 x 1000 tx |  7846 ms |
|                                |  8144 tx/s |  64 x 1000 tx |  7858 ms |
| `gcc_neon_orn_c4_ptr_u8_sd`    |  9614 tx/s |  128 x 500 tx |  6656 ms |
|                                |  9661 tx/s |  128 x 500 tx |  6624 ms |
|                                |  9664 tx/s |  128 x 500 tx |  6622 ms |
| `gcc_64_andn_c5_lut_u1_sd`     |  5620 tx/s |  64 x 1000 tx | 11387 ms |
|                                |  5626 tx/s |  64 x 1000 tx | 11373 ms |
|                                |  5613 tx/s |  64 x 1000 tx | 11400 ms |

## Samsung Grand Prime

Environment: ARMv8, Android 5.1.1, clang-8.0.0.

*Speedup*: 2.34x.

| Short config                   | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------- | ------------- | -------- |
| `clang_neon_orn_c4_ptr_u4_ss`  | 10127 tx/s |  128 x 500 tx |  6319 ms |
|                                | 10115 tx/s |  128 x 500 tx |  6327 ms |
|                                | 10111 tx/s |  128 x 500 tx |  6329 ms |
| `clang_neon_orn_c4_ptr_u8_sd`  |  9232 tx/s |  128 x 500 tx |  6932 ms |
|                                |  9225 tx/s |  128 x 500 tx |  6937 ms |
|                                |  9249 tx/s |  128 x 500 tx |  6919 ms |
| `clang_64_andn_c4_ptr_u4_ss`   |  5316 tx/s |  64 x 1000 tx | 12037 ms |
|                                |  5311 tx/s |  64 x 1000 tx | 12050 ms |
|                                |  5318 tx/s |  64 x 1000 tx | 12034 ms |
| `clang_64_orn_c4_ptr_u4_ss`    |  4712 tx/s |  64 x 1000 tx | 13580 ms |
|                                |  4707 tx/s |  64 x 1000 tx | 13596 ms |
|                                |  4720 tx/s |  64 x 1000 tx | 13558 ms |
| `clang_64_andn_c4_ptr_u8_sd`   |  4437 tx/s |  64 x 1000 tx | 14423 ms |
|                                |  4438 tx/s |  64 x 1000 tx | 14420 ms |
|                                |  4439 tx/s |  64 x 1000 tx | 14415 ms |
| `clang_64_andn_c5_lut_u1_sd`   |  4320 tx/s |  64 x 1000 tx | 14812 ms |
|                                |  4322 tx/s |  64 x 1000 tx | 14805 ms |
|                                |  4323 tx/s |  64 x 1000 tx | 14802 ms |

