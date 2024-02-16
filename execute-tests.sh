#! /bin/bash

available_dpus=$1
txtcyn='\e[0;36m' # Cyan Text
txtwht='\e[0;37m' # White Text

echo -e ${txtwht}Set available number of DPUs to $available_dpus

# create directory for test results
mkdir -p results

cd HElib-dpu

make clean-all

# HElib -------------------------------------------------------------------
# -- using SIMD methods
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib ...${txtwht}
make all -j8 NR_DPUS=$1 USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=0

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-0-0-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-0-0-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-0-0-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-0-0-$1.txt
./build/multiplications > ../results/res-multiplications-0-0-$1.txt

# -- using non-SIMD methods
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib ...${txtwht}
make all -j8 NR_DPUS=$1 USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-0-1-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-0-1-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-0-1-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-0-1-$1.txt
./build/multiplications > ../results/res-multiplications-0-1-$1.txt


# Using DPU ---------------------------------------------------------------
# -- using multi-tasklet implementations
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs ...${txtwht}
make all -j8 NR_DPUS=$1 USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1

# ---- using 8 tasklets
echo -e ${txtcyn}Building DPU programs using 8 DPU ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=8

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-1-8-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-1-8-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-1-8-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-1-8-$1.txt
./build/multiplications > ../results/res-multiplications-1-8-$1.txt

# ---- using 4 tasklets
echo -e ${txtcyn}Building DPU programs using 4 DPU ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=4

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-1-4-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-1-4-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-1-4-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-1-4-$1.txt
./build/multiplications > ../results/res-multiplications-1-4-$1.txt

# ---- using 2 tasklets
echo -e ${txtcyn}Building DPU programs using 2 DPU ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=2

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-1-2-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-1-2-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-1-2-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-1-2-$1.txt
./build/multiplications > ../results/res-multiplications-1-2-$1.txt

# ---- using 1 tasklet
echo -e ${txtcyn}Building DPU programs using 1 DPU ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=1

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-1-1-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-1-1-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-1-1-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-1-1-$1.txt
./build/multiplications > ../results/res-multiplications-1-1-$1.txt


# -- using single-tasklet implementations
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs ...${txtwht}
make all -j8 NR_DPUS=$1 USE_DPU=1 USE_MULTI_TASKLETS=0 PGFFT_DISABLE_SIMD=1

# ---- using 1 tasklet
echo -e ${txtcyn}Building DPU programs using 1 DPU ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=1

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/main > ../results/res-main-1-0-$1.txt
# ./build/BGV_binary_arithmetic > ../results/res-binary-1-0-$1.txt
# ./build/BGV_packed_arithmetic > ../results/res-packed-1-0-$1.txt
./build/BGV_country_db_lookup > ../results/res-country-1-0-$1.txt
./build/multiplications > ../results/res-multiplications-1-0-$1.txt

# all done
echo -e ${txtcyn}Successfully executed all test${txtwht}
