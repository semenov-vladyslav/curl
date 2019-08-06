cmake_minimum_required(VERSION 2.6)
project(ptrit_curl C)

option(PTRIT_PLATFORM "PTRIT_NEON PTRIT_AVX512 PTRIT_AVX2 PTRIT_AVX PTRIT_SSE2 PTRIT_SSE PTRIT_64" PTRIT_64)

add_definitions("-D${PTRIT_PLATFORM}")
add_executable(ptrit_curl ptrit_curl.c test.c)
