#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "common.h"

#define BUF_SIZE (MRAM_SIZE / 4 / sizeof(long))
#define C_SIZE (WRAM_SIZE) / NR_TASKLETS / 4 / sizeof(long)

__mram_noinit int64_t DPU_ROW_THIS[BUF_SIZE];
__mram_noinit int64_t DPU_ROW_OTHER[BUF_SIZE];
__host uint32_t DPU_SIZE;
__host int64_t DPU_PI;


/****************************************************************/
/* following methods are copied from NTL sp_arith.h             */

#define SP_CORRECT_EXCESS(a, n)        \
    {                                  \
        return a - n >= 0 ? a - n : a; \
    }

#define SP_CORRECT_DEFICIT(a, n)   \
    {                              \
        return a >= 0 ? a : a + n; \
    }

#define ADD_MOD(a, b, n)                    \
    {                                       \
        return SP_CORRECT_EXCESS(a + b, n); \
    }

#define SUB_MOD(a, b, n)                     \
    {                                        \
        return SP_CORRECT_DEFICIT(a - b, n); \
    }

#define NEGATE_MOD(a, n)         \
    {                            \
        return SUB_MOD(0, a, n); \
    }

long sp_CorrectExcess(long a, long n)
{
    return a - n >= 0 ? a - n : a;
}

long sp_CorrectDeficit(long a, long n)
{
    return a >= 0 ? a : a + n;
}

long AddMod(long a, long b, long n)
{
    long r = a + b;
    return sp_CorrectExcess(r, n);
}

long SubMod(long a, long b, long n)
{
    long r = a - b;
    return sp_CorrectDeficit(r, n);
}

long NegateMod(long a, long n)
{
    return SubMod(0, a, n);
}

/****************************************************************/

void ctxt_add(long start, long size)
{
    __dma_aligned long row_this[C_SIZE];
    __dma_aligned long row_other[C_SIZE];

    long _size = size < (C_SIZE) ? size : C_SIZE;
    long cache_copy_size = _size * sizeof(long);

    for (long i = start; i < start + size; i+=_size)
    {
        mram_read(&DPU_ROW_THIS[i], row_this, cache_copy_size);
        mram_read(&DPU_ROW_OTHER[i], row_other, cache_copy_size);

        for (long j = 0; j < _size; j++)
        {
            row_this[j] = AddMod(row_this[j], row_other[j], DPU_PI);
        }

        mram_write(row_this, &DPU_ROW_THIS[i], cache_copy_size);
    }
}

int main(void)
{
    uint8_t idx = me();

    // min. size per tasklet is 4 => we can use a maximum of "(size / 4) * blocks" DPUs
    uint8_t nr_tasklets = NR_TASKLETS;
    while (nr_tasklets > DPU_SIZE)
    {
        nr_tasklets /= 2;
    }

    if (idx >= nr_tasklets)
    {
        return 0;
    }

    printf(
        "me: %u\n  DPU_SIZE: %u - BUF_SIZE: %u - C_SIZE: %d - pi: %lld\n  size: %u - nr_tasklets %u\n",
        idx, DPU_SIZE, BUF_SIZE, C_SIZE, DPU_PI, DPU_SIZE / nr_tasklets, nr_tasklets);
    ctxt_add(idx * DPU_SIZE / nr_tasklets, DPU_SIZE / nr_tasklets);
    // DPU_ROW_THIS[10] = AddMod(DPU_ROW_THIS[0], DPU_ROW_OTHER[0], DPU_PI);

    return 0;
}
