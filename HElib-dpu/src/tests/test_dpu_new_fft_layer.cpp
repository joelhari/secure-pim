#include <complex>

#include "helib/PGFFT.h"
#include "test_utils.h"
#include "helib_pgfft_dpu.hpp"

typedef std::complex<double> cmplx_t;

#define TEST_SIZE 4096 // needs to be a multiple of 8
#define RANDOM_SEED 17

void init_random(cmplx_t *xs, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
    }
}

void test_dpu_new_fft_layer(long blocks, long size)
{
    // init arrays with random complex numbers
    cmplx_t xp[blocks * size];
    cmplx_t wtab[blocks * size];
    init_random(xp, blocks * size);
    init_random(wtab, blocks * size);

    cmplx_t orig_wtab[blocks * size];
    copy_complex_array(wtab, orig_wtab, blocks * size);

    // copy arrays for dpu execution
    cmplx_t dpu_xp[blocks * size];
    cmplx_t dpu_wtab[blocks * size];
    copy_complex_array(xp, dpu_xp, blocks * size);
    copy_complex_array(wtab, dpu_wtab, blocks * size);

    // check if copied arrays equal original arrays
    EQUALS_STD_COMPLEX_ARRAY(xp, dpu_xp, blocks * size);
    EQUALS_STD_COMPLEX_ARRAY(wtab, dpu_wtab, blocks * size);

    // perform helib new_fft_layer
    helib::PGFFT::test_new_fft_layer(xp, blocks, size, wtab);

    // perform dpu new_fft_layer
#ifdef USE_MULTI_TASKLETS
    dpu_new_fft_layer_2(dpu_xp, blocks, size, wtab);
#else
    dpu_new_fft_layer(dpu_xp, blocks, size, wtab);
#endif

    // check if arrays are equal, and wtab array are unmodified
    EQUALS_STD_COMPLEX_ARRAY(xp, dpu_xp, blocks * size);
    EQUALS_STD_COMPLEX_ARRAY(orig_wtab, wtab, blocks * size);
    EQUALS_STD_COMPLEX_ARRAY(orig_wtab, dpu_wtab, blocks * size);
    EQUALS_STD_COMPLEX_ARRAY(wtab, dpu_wtab, blocks * size);
}

int main(void)
{
#ifdef RANDOM_SEED
    std::srand(RANDOM_SEED);
#endif

    if (TEST_SIZE % 8 != 0)
    {
        std::cout << "    ERROR: invalid TEST_SIZE, needs to be a multiple of 8" << std::endl;
        return 0;
    }

    long blocks = 1;
    long size = TEST_SIZE;

    while (size >= 8)
    {
        std::cout << "running test (blocks: " << blocks << ", size: " << size << ")\r";
        std::cout.flush();

        test_dpu_new_fft_layer(blocks, size);
        
        blocks *= 2;
        size /= 2;
    }

    return 0;
}
