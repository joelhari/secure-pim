#include <complex>

#include "helib/CModulus.h"
#include "test_utils.h"
#include "helib_c_modulus_dpu.hpp"

#define NTL_BRC_Q (5) // needs to be the same as in CModulus.cpp
#define TEST_K 16
#define TEST_SIZE 1024 * (1L << (TEST_K - (2 * NTL_BRC_Q)))
#define TEST_RUNS 50
#define RANDOM_SEED 17

void init_random(long *A, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        A[i] = rand();
    }
}

void test_dpu_cobra()
{
    long A[TEST_SIZE];
    long B[TEST_SIZE];
    long dpu_B[TEST_SIZE];
    long k = TEST_K;
    long dpu_k = TEST_K;

    // init array A with random numbers
    init_random(A, TEST_SIZE);

    long orig_A[TEST_SIZE];
    copy_long_array(A, orig_A, TEST_SIZE);

    // copy array for dpu execution
    long dpu_A[TEST_SIZE];
    copy_long_array(A, dpu_A, TEST_SIZE);

    // check if copied arrays equal original arrays
    EQUALS_LONG_ARRAY(A, dpu_A, TEST_SIZE);
    EQUALS_LONG_ARRAY(A, orig_A, TEST_SIZE);

    // perform helib COBRA
    helib::Cmodulus::test_COBRA(B, A, k);

    // perform dpu COBRA
    helib::Cmodulus::test_DPU_COBRA(dpu_B, dpu_A, dpu_k);

    // check if arrays are equal, and A arrays are unmodified
    EQUALS_LONG_ARRAY(A, dpu_A, TEST_SIZE);
    EQUALS_LONG_ARRAY(B, dpu_B, TEST_SIZE);
    EQUALS_LONG_ARRAY(A, orig_A, TEST_SIZE);
}

int main(void)
{
#ifdef RANDOM_SEED
    std::srand(RANDOM_SEED);
#endif

    if (TEST_SIZE % 8 != 0)
    {
        std::cout << "    ERROR: invalid TEST_SIZE, needs to be a multiple of 8!" << std::endl;
        return 0;
    }

    for (size_t i = 0; i < TEST_RUNS; i++)
    {
        std::cout << "running test iteration " << i + 1 << "/" << TEST_RUNS << "\r";
        std::cout.flush();
        test_dpu_cobra();
    }

    return 0;
}
