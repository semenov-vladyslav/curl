@echo off

set tx_count=10000
set tx_count2=5000
set tx_count4=2500
set tx_count8=1250
set PCURL_DEBUG=OFF


set PATH=C:\Program Files\CMake\bin;C:\Windows\system32
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall1.bat" amd64 8.1

set cmake=cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
set build=nmake

mkdir .build_msvc_avx2_andn_c4_ptr_u4_ss
cd .build_msvc_avx2_andn_c4_ptr_u4_ss
%cmake% --config Release ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_AVX2 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 ^
  -DPCURL_STATE=PCURL_STATE_SHORT ^
  ..
%build%
pcurl %tx_count4% > pcurl.log
cd ..

mkdir .build_msvc_avx2_andn_c4_ptr_u2_sd
cd .build_msvc_avx2_andn_c4_ptr_u2_sd
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_AVX2 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_2 ^
  -DPCURL_STATE=PCURL_STATE_DOUBLE ^
  ..
%build%
pcurl %tx_count4% > pcurl.log
cd ..

mkdir .build_msvc_avx2_andn_c4_ptr_u4_sd
cd .build_msvc_avx2_andn_c4_ptr_u4_sd
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_AVX2 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 ^
  -DPCURL_STATE=PCURL_STATE_DOUBLE ^
  ..
%build%
pcurl %tx_count4% > pcurl.log
cd ..

mkdir .build_msvc_avx2_andn_c4_ptr_u8_sd
cd .build_msvc_avx2_andn_c4_ptr_u8_sd
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_AVX2 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_8 ^
  -DPCURL_STATE=PCURL_STATE_DOUBLE ^
  ..
%build%
pcurl %tx_count4% > pcurl.log
cd ..

mkdir .build_msvc_sse2_andn_c4_ptr_u4_ss
cd .build_msvc_sse2_andn_c4_ptr_u4_ss
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_SSE2 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 ^
  -DPCURL_STATE=PCURL_STATE_SHORT ^
  ..
%build%
pcurl %tx_count2% > pcurl.log
cd ..

mkdir .build_msvc_64_andn_c4_ptr_u4_ss
cd .build_msvc_64_andn_c4_ptr_u4_ss
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_64 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT4 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_PTR ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_4 ^
  -DPCURL_STATE=PCURL_STATE_SHORT ^
  ..
%build%
pcurl %tx_count% > pcurl.log
cd ..

mkdir .build_msvc_64_andn_c5_lut_u1_sd
cd .build_msvc_64_andn_c5_lut_u1_sd
%cmake% ^
  -DPCURL_DEBUG=%PCURL_DEBUG% ^
  -DPTRIT_PLATFORM=PTRIT_64 ^
  -DPTRIT_CVT=PTRIT_CVT_ANDN ^
  -DPCURL_S2_CIRCUIT=PCURL_S2_CIRCUIT5 ^
  -DPCURL_S2_ARGS=PCURL_S2_ARGS_LUT ^
  -DPCURL_SBOX_UNWIND=PCURL_SBOX_UNWIND_1 ^
  -DPCURL_STATE=PCURL_STATE_DOUBLE ^
  ..
%build%
pcurl %tx_count% > pcurl.log
cd ..

for /d %%d in (.build_msvc*) do (
echo %%d
type %%d\pcurl.log
)
