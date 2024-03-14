#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "common.h"

// #ifndef NR_TASKLETS
// #define NR_TASKLETS 1
// #endif

#define D_BUFFER_SIZE MRAM_SIZE / sizeof(int64_t) / 8
#define D_CACHE_SIZE WRAM_SIZE / sizeof(int64_t) / 5

__mram_noinit int64_t DPU_A[D_BUFFER_SIZE];
__mram_noinit int64_t DPU_B[D_BUFFER_SIZE];
__mram_noinit int64_t DPU_T[D_BUFFER_SIZE];
__mram_noinit int64_t DPU_REV_Q[D_BUFFER_SIZE];
__host int64_t DPU_Q;

void cobra(uint32_t mram_read_start)
{
    __dma_aligned int64_t A[D_CACHE_SIZE];
    __dma_aligned int64_t B[D_CACHE_SIZE];
    // __dma_aligned long T[D_CACHE_SIZE];
    __dma_aligned int64_t rev_q[D_CACHE_SIZE];

    int64_t a1, c1;
    int64_t cache_copy_size = (1L << DPU_Q) * sizeof(int64_t);

    for (long a = 0; a < (1L << DPU_Q); a++)
    {
        a1 = DPU_REV_Q[a];

        mram_read(&DPU_A[a * (1L << DPU_Q)], A, cache_copy_size);

        for (long c = 0; c < (1L << DPU_Q); c++)
        {
            DPU_T[(a1 << DPU_Q) + c] = A[c];
        }
    }

    for (long c = 0; c < (1L << DPU_Q); c++)
    {
        c1 = DPU_REV_Q[c];

        for (long a1 = 0; a1 < (1L << DPU_Q); a1++)
        {
            B[a1] = DPU_T[(a1 << DPU_Q) + c];
        }

        mram_write(B, &DPU_B[c * (1L << DPU_Q)], cache_copy_size);
    }
}

int main(void)
{
    uint8_t idx = me();

    // min. size per tasklet is 4 => we can use a maximum of "(size / 4) * blocks" DPUs
    uint8_t nr_tasklets = 1; // NR_TASKLETS;
    // while (nr_tasklets > DPU_SIZE)
    // {
    //     nr_tasklets /= 2;
    // }

    // uint32_t per_tasklet_size = DPU_SIZE / nr_tasklets;

    if (idx >= nr_tasklets)
    {
        return 0;
    }

    cobra(0);

    return 0;
}
