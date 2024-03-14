
#ifndef _H_HELIB_C_MODULUS_DPU
#define _H_HELIB_C_MODULUS_DPU

#include <helib/CModulus.h>

void dpu_BasicBitReverseCopy(
    long* B,
    const long* A,
    long k,
    long *rev);

void dpu_COBRA(
    long *B,
    const long *A,
    long k1,
    long *rev_k1,
    long *rev_q,
    long q);

// void dpu_BitReverseCopy(long* NTL_RESTRICT B, const long* NTL_RESTRICT A, long k);

#endif // _H_HELIB_C_MODULUS_DPU
