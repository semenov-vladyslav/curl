#!/bin/sh

#export CC=clang ./bench_arm.sh

mkdir .build_${CC}_neon_orn_opt
cd .build_${CC}_neon_orn_opt
cmake -G "Unix Makefiles" -DPTRIT_PLATFORM=PTRIT_NEON -DPTRIT_CVT=PTRIT_CVT_ORN -DPCURL_SBOX=PCURL_SBOX_OPT ..
./pcurl
cd ..

mkdir .build_${CC}_neon_orn_mem2x
cd .build_${CC}_neon_orn_mem2x
cmake -G "Unix Makefiles" -DPTRIT_PLATFORM=PTRIT_NEON -DPTRIT_CVT=PTRIT_CVT_ORN -DPCURL_SBOX=PCURL_SBOX_MEM2X ..
./pcurl
cd ..

mkdir .build_${CC}_64_andn_opt
cd .build_${CC}_64_andn_opt
cmake -G "Unix Makefiles" -DPTRIT_PLATFORM=PTRIT_64 -DPTRIT_CVT=PTRIT_CVT_ANDN -DPCURL_SBOX=PCURL_SBOX_OPT ..
./pcurl
cd ..

mkdir .build_${CC}_64_andn_mem2x
cd .build_${CC}_64_andn_mem2x
cmake -G "Unix Makefiles" -DPTRIT_PLATFORM=PTRIT_64 -DPTRIT_CVT=PTRIT_CVT_ANDN -DPCURL_SBOX=PCURL_SBOX_MEM2X ..
./pcurl
cd ..

mkdir .build_${CC}_64_andn_index
cd .build_${CC}_64_andn_index
cmake -G "Unix Makefiles" -DPTRIT_PLATFORM=PTRIT_64 -DPTRIT_CVT=PTRIT_CVT_ANDN -DPCURL_SBOX=PCURL_SBOX_INDEX ..
./pcurl
cd ..

