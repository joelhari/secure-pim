#include <complex>

#include "helib/CModulus.h"
#include "test_utils.h"
#include "helib_c_modulus_dpu.hpp"

#define TEST_RUNS 100
#define RANDOM_SEED 17

void init_random(long *A, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        A[i] = rand();
    }
}

void test_dpu_basic_bit_reverse_copy()
{
    long k = 11;
    long dpu_k = k;
    long n = 1L << k;
    long A[n];
    long B[n];
    long dpu_B[n];

    // init array A with random numbers
    init_random(A, n);

    long orig_A[n];
    copy_long_array(A, orig_A, n);

    // copy array for dpu execution
    long dpu_A[n];
    copy_long_array(A, dpu_A, n);

    // check if copied arrays equal original arrays
    EQUALS_LONG_ARRAY(A, dpu_A, n);
    EQUALS_LONG_ARRAY(A, orig_A, n);

    // perform helib BasicBitReverseCopy
    helib::Cmodulus::test_BasicBitReverseCopy(B, A, k);

    // perform dpu BasicBitReverseCopy
    helib::Cmodulus::test_DPU_BasicBitReverseCopy(dpu_B, dpu_A, dpu_k);

    // check if arrays are equal, and A arrays are unmodified
    EQUALS_LONG_ARRAY(A, dpu_A, n);
    EQUALS_LONG_ARRAY(B, dpu_B, n);
    EQUALS_LONG_ARRAY(A, orig_A, n);
}

int main(void)
{
#ifdef RANDOM_SEED
    std::srand(RANDOM_SEED);
#endif

    for (size_t i = 0; i < TEST_RUNS; i++)
    {
        std::cout << "running test iteration " << i + 1 << "/" << TEST_RUNS << "\r";
        std::cout.flush();
        test_dpu_basic_bit_reverse_copy();
    }

    return 0;
}
