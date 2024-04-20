#! /bin/bash

available_dpus=$1
txtcyn='\e[0;36m' # Cyan Text
txtwht='\e[0;37m' # White Text

echo -e ${txtwht}Set available number of DPUs to $available_dpus

# create directory for test results
mkdir -p results-add

cd HElib-dpu

make clean-all


# Build DPU programs ------------------------------------------------------
echo -e ${txtcyn}Building DPU programs using 16 tasklets ...${txtwht}
make clean-dpu
make dpu -j8 NR_TASKLETS=16

# CKKS Context 20 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=20 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=20

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-20.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-20.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=20 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=20

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-20.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-20.txt


# CKKS Context 30 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=30 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=30

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-30.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-30.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=30 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=30

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-30.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-30.txt


# CKKS Context 31 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=31 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=31

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-31.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-31.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=31 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=31

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-31.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-31.txt


# CKKS Context 32 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=32 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=32

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-32.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-32.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=32 ...${txtwht}
make all -j8 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=32

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-32.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-32.txt


# all done
echo -e ${txtcyn}Successfully executed all test${txtwht}
