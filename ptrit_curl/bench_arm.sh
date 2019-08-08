#!/bin/sh

#export CC=clang ./bench_arm.sh
export PCURL_DEBUG=OFF

mkdir .build_${CC}_neon_orn_c4_ptr_u4_ss
cd .build_${CC}_neon_orn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_NEON \
  -DPTRIT_CVT=PTRIT_CVT_ORN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl > pcurl.log
cd ..

mkdir .build_${CC}_neon_orn_c4_ptr_u8_sd
cd .build_${CC}_neon_orn_c4_ptr_u8_sd
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_NEON \
  -DPTRIT_CVT=PTRIT_CVT_ORN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_8 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl > pcurl.log
cd ..

mkdir .build_${CC}_64_orn_c4_ptr_u4_ss
cd .build_${CC}_64_orn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ORN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl > pcurl.log
cd ..

mkdir .build_${CC}_64_andn_c4_ptr_u4_ss
cd .build_${CC}_64_andn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl > pcurl.log
cd ..

mkdir .build_${CC}_64_andn_c4_ptr_u8_sd
cd .build_${CC}_64_andn_c4_ptr_u8_sd
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_8 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl > pcurl.log
cd ..

mkdir .build_${CC}_64_andn_c5_lut_u1_sd
cd .build_${CC}_64_andn_c5_lut_u1_sd
cmake -G "Unix Makefiles" \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT5 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_LUT \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_1 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl > pcurl.log
cd ..

find . -name pcurl.log -print -exec tail -n 3 {} \;
