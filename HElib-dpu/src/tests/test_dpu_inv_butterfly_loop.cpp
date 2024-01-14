#include <complex>

#include "helib/PGFFT.h"
#include "test_utils.h"
#include "helib_pgfft_dpu.hpp"

typedef std::complex<double> cmplx_t;

#define TEST_SIZE 2048 // needs to be a multiple of 4
#define TEST_RUNS 10
#define RANDOM_SEED 17

void init_random(cmplx_t *xs, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
    }
}

void test_dpu_inv_butterfly_loop(void)
{
    // init arrays with random complex numbers
    cmplx_t xp0[TEST_SIZE];
    cmplx_t xp1[TEST_SIZE];
    cmplx_t wtab[TEST_SIZE];
    init_random(xp0, TEST_SIZE);
    init_random(xp1, TEST_SIZE);
    init_random(wtab, TEST_SIZE);

    cmplx_t orig_wtab[TEST_SIZE];
    copy_complex_array(wtab, orig_wtab, TEST_SIZE);

    // copy arrays for dpu execution
    cmplx_t dpu_xp0[TEST_SIZE];
    cmplx_t dpu_xp1[TEST_SIZE];
    cmplx_t dpu_wtab[TEST_SIZE];
    copy_complex_array(xp0, dpu_xp0, TEST_SIZE);
    copy_complex_array(xp1, dpu_xp1, TEST_SIZE);
    copy_complex_array(wtab, dpu_wtab, TEST_SIZE);

    // check if copied arrays equal original arrays
    EQUALS_STD_COMPLEX_ARRAY(xp0, dpu_xp0, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(xp1, dpu_xp1, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(wtab, dpu_wtab, TEST_SIZE);

    // perform helib inv_butterfly_loop
    helib::PGFFT::test_inv_butterfly_loop(TEST_SIZE, xp0, xp1, wtab);

    // perform dpu inv_butterfly_loop
#ifdef USE_MULTI_TASKLETS
    dpu_inv_butterfly_loop_2(TEST_SIZE, dpu_xp0, dpu_xp1, dpu_wtab);
#else
    dpu_inv_butterfly_loop(TEST_SIZE, dpu_xp0, dpu_xp1, dpu_wtab);
#endif

    // check if arrays are equal, and wtab array are unmodified
    EQUALS_STD_COMPLEX_ARRAY(xp0, dpu_xp0, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(xp1, dpu_xp1, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(orig_wtab, wtab, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(orig_wtab, dpu_wtab, TEST_SIZE);
    EQUALS_STD_COMPLEX_ARRAY(wtab, dpu_wtab, TEST_SIZE);
}

int main(void)
{
#ifdef RANDOM_SEED
    std::srand(RANDOM_SEED);
#endif

    if (TEST_SIZE % 4 != 0)
    {
        std::cout << "    ERROR: invalid TEST_SIZE, needs to be a multiple of 4!" << std::endl;
        return 0;
    }

    for (size_t i = 0; i < TEST_RUNS; i++)
    {
        std::cout << "running test iteration " << i + 1 << "/" << TEST_RUNS << "\r";
        std::cout.flush();
        test_dpu_inv_butterfly_loop();
    }

    return 0;
}
