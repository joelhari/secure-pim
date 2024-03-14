#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "common.h"

#ifndef NR_TASKLETS
#define NR_TASKLETS 1
#endif

#define D_BUFFER_SIZE MRAM_SIZE / sizeof(int64_t) / 8
#define D_CACHE_SIZE WRAM_SIZE / sizeof(int64_t) / 4

__mram_noinit int64_t DPU_A[D_BUFFER_SIZE];
__mram_noinit int64_t DPU_B[D_BUFFER_SIZE];
__mram_noinit int64_t DPU_REV_Q[D_BUFFER_SIZE];
__host int64_t DPU_SIZE;

void basic_bit_reverse_copy(uint32_t mram_read_start)
{
    for (long i = 0; i < DPU_SIZE; i++)
    {
        DPU_B[DPU_REV_Q[i]] = DPU_A[i];
    }

    // __dma_aligned int64_t A[D_CACHE_SIZE];
    // __dma_aligned int64_t rev[D_CACHE_SIZE];

    // int32_t cache_copy_size = DPU_SIZE * sizeof(int64_t) * 4;
    // // uint32_t iterations = cache_copy_size /

    // printf("DPU_SIZE %lld\n", DPU_SIZE);
    // printf("cache_copy_size %d\n", cache_copy_size);

    // mram_read(&DPU_A[0], &A[0], cache_copy_size);
    // mram_read(&DPU_REV_Q[0], &rev[0], cache_copy_size);

    // // uint32_t i = 0;

    // for (long i = 0; i < DPU_SIZE; i++)
    // {
    //     // printf("A[%ld] = %lld - rev[%ld] = %lld\n", i, A[i], i, rev[i]);
    //     DPU_B[rev[i]] = A[i];
    // }

    // mram_write(B, DPU_B, DPU_SIZE * sizeof(int64_t));
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

    basic_bit_reverse_copy(0);

    return 0;
}
