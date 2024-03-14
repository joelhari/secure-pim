#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "dpu_complex_operations.h"
#include "common.h"

typedef dpu_complex complex;

__mram_noinit complex DPU_XP0[BUFFER_SIZE];
__mram_noinit complex DPU_XP1[BUFFER_SIZE];
__mram_noinit complex DPU_WTAB[BUFFER_SIZE];
__host uint32_t DPU_BLOCKS;
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
    uint32_t mram_read_start, const uint32_t per_tasklet_size, uint8_t do_butterfly0
)
{
    __dma_aligned complex xp0[LOCAL_CACHE_SIZE];
    __dma_aligned complex xp1[LOCAL_CACHE_SIZE];
    __dma_aligned complex wtab[LOCAL_CACHE_SIZE];

    mram_read(&DPU_XP0[mram_read_start], xp0, LOCAL_CACHE_SIZE);
    mram_read(&DPU_XP1[mram_read_start], xp1, LOCAL_CACHE_SIZE);
    mram_read(&DPU_WTAB[mram_read_start], wtab, LOCAL_CACHE_SIZE);


    if (do_butterfly0 == 1)
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

    mram_write(xp0, &DPU_XP0[mram_read_start], LOCAL_CACHE_SIZE);
    mram_write(xp1, &DPU_XP1[mram_read_start], LOCAL_CACHE_SIZE);

    for (uint32_t j = 4; j < per_tasklet_size; j += 4)
    {
        mram_read_start += 4;

        mram_read(&DPU_XP0[mram_read_start], xp0, LOCAL_CACHE_SIZE);
        mram_read(&DPU_XP1[mram_read_start], xp1, LOCAL_CACHE_SIZE);
        mram_read(&DPU_WTAB[mram_read_start], wtab, LOCAL_CACHE_SIZE);

        inv_butterfly(xp0[0], xp1[0], wtab[0]);
        inv_butterfly(xp0[1], xp1[1], wtab[1]);
        inv_butterfly(xp0[2], xp1[2], wtab[2]);
        inv_butterfly(xp0[3], xp1[3], wtab[3]);

        mram_write(xp0, &DPU_XP0[mram_read_start], LOCAL_CACHE_SIZE);
        mram_write(xp1, &DPU_XP1[mram_read_start], LOCAL_CACHE_SIZE);
    }
}

int main(void)
{
    uint8_t idx = me();

    // min. size per tasklet is 4 => we can use a maximum of "(size / 4) * blocks" DPUs
    uint8_t nr_tasklets = NR_TASKLETS;
    while (nr_tasklets > DPU_BLOCKS * DPU_SIZE / 4)
    {
        nr_tasklets /= 2;
    }

    uint32_t per_tasklet_blocks = DPU_BLOCKS / nr_tasklets;
    uint8_t per_block_tasklets = nr_tasklets / DPU_BLOCKS;
    uint32_t per_tasklet_size = DPU_SIZE / (per_block_tasklets > 0 ? per_block_tasklets : 1);

    if (idx >= nr_tasklets)
    {
        return 0;
    }

    if (per_block_tasklets > 0) {
        uint32_t block_idx = idx / per_block_tasklets;
        uint32_t mram_read_start = block_idx * DPU_SIZE + (idx % per_block_tasklets) * per_tasklet_size;
        uint8_t do_butterfly0;

        if (DPU_BLOCKS > 1)
        {
            do_butterfly0 = idx % per_block_tasklets == 0 ? 1 : 0;
        }
        else
        {
            do_butterfly0 = DPU_DO_INV_BUTTERFLY0 == 1 && idx == 0 ? 1 : 0;
        }

        inv_butterfly_loop(mram_read_start, per_tasklet_size, do_butterfly0);
    }
    else
    {
        for (uint8_t i = 0; i < per_tasklet_blocks; i++)
        {
            uint32_t mram_read_start = idx * (per_tasklet_size * per_tasklet_blocks) + i * per_tasklet_size;
            inv_butterfly_loop(mram_read_start, per_tasklet_size, 1);
        }
    }

    return 0;
}
