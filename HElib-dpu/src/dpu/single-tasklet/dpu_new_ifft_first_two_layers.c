#include <stdio.h>
#include <mram.h>
#include "dpu_complex.h"
#include "common.h"

typedef dpu_complex complex;

__mram_noinit complex DPU_XP[BUFFER_SIZE];

__host uint32_t DPU_BLOCKS;

void new_ifft_first_two_layers(/* complex *xp, long blocks, const complex *wtab */)
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
    } while (--DPU_BLOCKS != 0);
}

int main(void)
{
    new_ifft_first_two_layers();

    return 0;
}