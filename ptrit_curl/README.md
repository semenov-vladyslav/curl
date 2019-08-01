# ptrit_curl

Fast reference implementation of vectorized IOTA Curl transform function; tests and benchmarks.

# Build and Run

On Windows:
```
mkdir .build
pushd .build
cmake -G "Visual Studio 15 2017 Win64" ..
popd
```
On Linux:
```
mkdir .build
cd .build
cmake -G "Unix Makefiles" ..
make
```

# Benchmark results

Environment: Intel Core i7 4510U, Windows 8.1, msvc-14.1, cmake, Release build. 
Time is measured using `clock` function, benchmark results have quite significant variance due to platform specifics (mobile PC, noisy environment).
Time presented is "average" of several benchmark runs.
TODO: Run benchmarks for gcc builds.

## Test implementations
* `ptrit_curl_sbox` -- new algorithm, can be built with SIMD extensions (SSE2, AVX2, AVX512), or without.
* `ptrit_curl_sbox_64` -- current `ptrit_curl` s-box algorithm, implemented in `entangled`; doesn't make use of SIMD extensions.
* `ptrit_curl_sbox_dcurl` -- slightly rewritten algorithm from `https://github.com/DLTcollab/dcurl`, initially implemented to support SIMD extensions.

## PTRIT_64
Native build, without any SIMD extensions. Each s-box transformation is called 3300000 times (equiv to 6400K transactions), time is in ms.
```
ptrit_curl_sbox       = 2453 ms
ptrit_curl_sbox_64    = 4415 ms
ptrit_curl_sbox_dcurl = 4422 ms
```

## PTRIT_AVX2
Build, with AVX2 enabled. Each s-box transformation is called 3300000/4 times (equiv to 6400K transactions), time is in ms.
```
ptrit_curl_sbox       = 1760 ms
ptrit_curl_sbox_dcurl = 1881 ms
```

