#! /bin/bash

available_dpus=$1
txtcyn='\e[0;36m' # Cyan Text
txtwht='\e[0;37m' # White Text

echo -e ${txtwht}Set available number of DPUs to $available_dpus

# create directory for test results
mkdir -p results-fft-layer

cd HElib-dpu

make clean-all


# Build DPU programs ------------------------------------------------------
echo -e ${txtcyn}Building DPU programs using 8 tasklets ...${txtwht}
make clean-dpu
make dpu -j40 NR_TASKLETS=8


# CKKS Context 0 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=0 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=0

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-fft-layer/res-fft-layer-0-1-$available_dpus-0.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=0 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=0

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-fft-layer/res-fft-layer-1-8-$available_dpus-0.txt


# all done
echo -e ${txtcyn}Successfully executed all test${txtwht}
