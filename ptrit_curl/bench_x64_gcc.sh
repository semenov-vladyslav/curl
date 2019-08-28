#!/bin/sh

export tx_count=10000
export tx_count2=5000
export tx_count4=2500
export tx_count8=1250
export PCURL_DEBUG=OFF


mkdir .build_gcc_avx2_andn_c4_ptr_u8_sd
cd .build_gcc_avx2_andn_c4_ptr_u8_sd
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_AVX2 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_8 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl $tx_count4 > pcurl.log
cd ..

mkdir .build_gcc_avx2_andn_c4_ptr_u2_sd
cd .build_gcc_avx2_andn_c4_ptr_u2_sd
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_AVX2 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_2 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl $tx_count4 > pcurl.log
cd ..


mkdir .build_gcc_avx512_andn_c4_ptr_u4_ss
cd .build_gcc_avx512_andn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release --config Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_AVX512 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
sde ./pcurl -- 1 > pcurl.log
cd ..

mkdir .build_gcc_avx2_andn_c4_ptr_u4_ss
cd .build_gcc_avx2_andn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release --config Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_AVX2 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl $tx_count4 > pcurl.log
cd ..

mkdir .build_gcc_avx2_andn_c4_ptr_u4_sd
cd .build_gcc_avx2_andn_c4_ptr_u4_sd
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_AVX2 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl $tx_count4 > pcurl.log
cd ..

mkdir .build_gcc_sse2_andn_c4_ptr_u4_ss
cd .build_gcc_sse2_andn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_SSE2 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl $tx_count2 > pcurl.log
cd ..

mkdir .build_gcc_64_andn_c4_ptr_u4_ss
cd .build_gcc_64_andn_c4_ptr_u4_ss
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 \
  -DPCURL_STATE=PCURL_STATE_SHORT \
  ..
make
./pcurl $tx_count > pcurl.log
cd ..

mkdir .build_gcc_64_andn_c5_lut_u1_sd
cd .build_gcc_64_andn_c5_lut_u1_sd
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
  -DPCURL_DEBUG=$PCURL_DEBUG \
  -DPTRIT_PLATFORM=PTRIT_64 \
  -DPTRIT_CVT=PTRIT_CVT_ANDN \
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT5 \
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_LUT \
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_1 \
  -DPCURL_STATE=PCURL_STATE_DOUBLE \
  ..
make
./pcurl $tx_count > pcurl.log
cd ..

find . -name pcurl.log -print -exec tail -n 3 {} \;
