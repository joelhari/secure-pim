#include <stdio.h>
#include <mram.h>
#include "dpu_complex.h"
#include "common.h"

typedef dpu_complex complex;

__mram_noinit complex DPU_XP0[BUFFER_SIZE];
__mram_noinit complex DPU_XP1[BUFFER_SIZE];
__mram_noinit complex DPU_WTAB[BUFFER_SIZE];
__host uint32_t DPU_SIZE;
__host uint32_t DPU_DO_INV_BUTTERFLY0;

#define inv_butterfly(xx0, xx1, w) \
    do                             \
    {                              \
        complex x0_ = xx0;         \
        complex x1_ = xx1;         \
        complex t_ = cmul(x1_, w); \
        xx0 = add(x0_, t_);        \
        xx1 = sub(x0_, t_);        \
    } while (0)

#define inv_butterfly0(xx0, xx1) \
    do                           \
    {                            \
        complex x0_ = xx0;       \
        complex x1_ = xx1;       \
        xx0 = add(x0_, x1_);     \
        xx1 = sub(x0_, x1_);     \
    } while (0)

void inv_butterfly_loop(
    /* long size,
    complex *restrict xp0,
    complex *restrict xp1,
    const complex *restrict wtab */
)
{
    __dma_aligned complex xp0[LOCAL_CACHE_SIZE];
    __dma_aligned complex xp1[LOCAL_CACHE_SIZE];
    __dma_aligned complex wtab[LOCAL_CACHE_SIZE];

    mram_read(&DPU_XP0[0], xp0, LOCAL_CACHE_SIZE);
    mram_read(&DPU_XP1[0], xp1, LOCAL_CACHE_SIZE);
    mram_read(&DPU_WTAB[0], wtab, LOCAL_CACHE_SIZE);

    if (DPU_DO_INV_BUTTERFLY0 == 1)
    {
        inv_butterfly0(xp0[0 + 0], xp1[0 + 0]);
    }
    else
    {
        inv_butterfly(xp0[0 + 0], xp1[0 + 0], wtab[0 + 0]);
    }

    inv_butterfly(xp0[0 + 1], xp1[0 + 1], wtab[0 + 1]);
    inv_butterfly(xp0[0 + 2], xp1[0 + 2], wtab[0 + 2]);
    inv_butterfly(xp0[0 + 3], xp1[0 + 3], wtab[0 + 3]);

    mram_write(xp0, &DPU_XP0[0], LOCAL_CACHE_SIZE);
    mram_write(xp1, &DPU_XP1[0], LOCAL_CACHE_SIZE);

    for (uint32_t j = BLOCK_SIZE; j < DPU_SIZE; j += BLOCK_SIZE)
    {
        mram_read(&DPU_XP0[j], xp0, LOCAL_CACHE_SIZE);
        mram_read(&DPU_XP1[j], xp1, LOCAL_CACHE_SIZE);
        mram_read(&DPU_WTAB[j], wtab, LOCAL_CACHE_SIZE);

        inv_butterfly(xp0[0], xp1[0], wtab[0]);
        inv_butterfly(xp0[1], xp1[1], wtab[1]);
        inv_butterfly(xp0[2], xp1[2], wtab[2]);
        inv_butterfly(xp0[3], xp1[3], wtab[3]);

        mram_write(xp0, &DPU_XP0[j], LOCAL_CACHE_SIZE);
        mram_write(xp1, &DPU_XP1[j], LOCAL_CACHE_SIZE);
    }
}

int main(void)
{
    inv_butterfly_loop();

    return 0;
}
