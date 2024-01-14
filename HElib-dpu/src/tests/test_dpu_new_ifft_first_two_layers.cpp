#include <complex>

#include "helib/PGFFT.h"
#include "test_utils.h"
#include "helib_pgfft_dpu.hpp"

typedef std::complex<double> cmplx_t;

#define TEST_BLOCKS 512
#define TEST_SIZE TEST_BLOCKS * 4
#define TEST_RUNS 100
#define RANDOM_SEED 17

void init_random(cmplx_t *xs, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
    }
}

void test_dpu_new_ifft_first_two_layers(void)
{
    // init array with random complex numbers
    cmplx_t xp[TEST_SIZE];
    init_random(xp, TEST_SIZE);

    // copy array for dpu execution
    cmplx_t dpu_xp[TEST_SIZE];
    copy_complex_array(xp, dpu_xp, TEST_SIZE);

    // check if copied array equal original array
    EQUALS_STD_COMPLEX_ARRAY(xp, dpu_xp, TEST_SIZE);

    // perform helib new_ifft_first_two_layers
    helib::PGFFT::test_new_ifft_first_two_layers(xp, TEST_BLOCKS);

    // perform dpu new_ifft_first_two_layers
#ifdef USE_MULTI_TASKLETS
    dpu_new_ifft_first_two_layers_2(dpu_xp, TEST_BLOCKS);
#else
    dpu_new_ifft_first_two_layers(dpu_xp, TEST_BLOCKS);
#endif

    // check if arrays are equal
    EQUALS_STD_COMPLEX_ARRAY(xp, dpu_xp, TEST_SIZE);
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
        test_dpu_new_ifft_first_two_layers();
    }

    return 0;
}
