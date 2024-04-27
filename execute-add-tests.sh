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
make dpu -j40 NR_TASKLETS=16

# CKKS Context 41 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=41 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=41

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-41.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-41.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=41 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=41

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-41.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-41.txt


# CKKS Context 42 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=42 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=42

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-42.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-42.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=42 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=42

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-42.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-42.txt


# CKKS Context 43 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=43 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=43

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-43.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-43.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=43 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=43

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-43.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-43.txt


# CKKS Context 44 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=44 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=44

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-44.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-44.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=44 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=44

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-44.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-44.txt


# CKKS Context 45 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=45 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=45

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-45.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-45.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=45 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=45

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-45.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-45.txt


# CKKS Context 46 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=46 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=46

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-46.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-46.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=46 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=46

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-46.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-46.txt


# CKKS Context 47 ---------------------------------------------------------

# ---- HElib implementation -----------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib - CKKS_CONTEXT_IDX=47 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=0 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=47

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-0-1-$available_dpus-47.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-0-1-$available_dpus-47.txt


# ---- DPU implementation -------------------------------------------------
echo -e ${txtcyn}Cleaning ...${txtwht}
make clean

echo -e ${txtcyn}Building HElib using DPUs - CKKS_CONTEXT_IDX=47 ...${txtwht}
make all -j40 NR_DPUS=$available_dpus USE_DPU=1 USE_MULTI_TASKLETS=1 PGFFT_DISABLE_SIMD=1 CKKS_CONTEXT_IDX=47

echo -e ${txtcyn}Running Tests ...${txtwht}
./build/additions > ../results-add/res-additions-1-16-$available_dpus-47.txt
./build/BGV_binary_arithmetic > ../results-add/res-binary-1-16-$available_dpus-47.txt


# all done
echo -e ${txtcyn}Successfully executed all test${txtwht}
