#include <stdio.h>
#include <defs.h>
#include <mram.h>
#include "dpu_complex_operations.h"
#include "common.h"

typedef dpu_complex complex;

__mram_noinit complex DPU_XP[BUFFER_SIZE];

__host uint32_t DPU_BLOCKS;

void new_ifft_first_two_layers(
    /* complex *xp, long blocks, const complex *wtab */
    uint32_t mram_read_start, uint32_t blocks)
{
    __dma_aligned complex xp[LOCAL_CACHE_SIZE];

    uint32_t i = mram_read_start;

    do
    {
        mram_read(&DPU_XP[i], xp, LOCAL_CACHE_SIZE);

        complex u0 = xp[0];
        complex u1 = xp[1];
        complex u2 = xp[2];
        complex u3 = xp[3];

        complex v0 = add(u0, u1);
        complex v1 = sub(u0, u1);
        complex v2 = add(u2, u3);
        complex t = sub(u2, u3);

        // cmplx_t v3 = CMUL(t, w);
        //  DIRT: relies on w == (0,1)
        complex v3 = { -t.imag, t.real };

        xp[0] = add(v0, v2);
        xp[2] = sub(v0, v2);
        xp[1] = add(v1, v3);
        xp[3] = sub(v1, v3);

        mram_write(xp, &DPU_XP[i], LOCAL_CACHE_SIZE);

        i += BLOCK_SIZE;
    } while (--blocks != 0);
}

int main(void)
{
    uint8_t idx = me();

    // min. blocks per tasklet is 1 => we can use a maximum of "blocks" DPUs
    uint8_t nr_tasklets = NR_TASKLETS;
    while (nr_tasklets > DPU_BLOCKS)
    {
        nr_tasklets /= 2;
    }

    uint32_t per_tasklet_blocks = DPU_BLOCKS / nr_tasklets;
    uint32_t mram_read_start = idx * per_tasklet_blocks * BLOCK_SIZE;

    new_ifft_first_two_layers(mram_read_start, per_tasklet_blocks);

    return 0;
}