#include <stdio.h>
#include <mram.h>
#include "dpu_complex_operations.h"
#include "common.h"

typedef dpu_complex complex;

__mram_noinit complex DPU_XP[BUFFER_SIZE];

__host uint32_t DPU_BLOCKS;

void new_fft_last_two_layers(/* complex *xp, long blocks, const complex *wtab */)
{
    __dma_aligned complex xp[LOCAL_CACHE_SIZE];

    uint32_t i = 0;

    do
    {
        mram_read(&DPU_XP[i], xp, LOCAL_CACHE_SIZE);

        complex u0 = xp[0];
        complex u1 = xp[1];
        complex u2 = xp[2];
        complex u3 = xp[3];

        complex v0 = add(u0, u2);
        complex v2 = sub(u0, u2);
        complex v1 = add(u1, u3);
        complex t = sub(u1, u3);

        // complex v3 = MUL(t, w);
        //  DIRT: relies on w == (0,-1)
        complex v3 = { t.imag, -t.real };

        xp[0] = add(v0, v1);
        xp[1] = sub(v0, v1);
        xp[2] = add(v2, v3);
        xp[3] = sub(v2, v3);

        mram_write(xp, &DPU_XP[i], LOCAL_CACHE_SIZE);

        i += BLOCK_SIZE;
    } while (--DPU_BLOCKS != 0);
}

int main(void)
{
    new_fft_last_two_layers();

    return 0;
}