#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "common.h"


/****************************************************************/
/* following methods are copied from NTL sp_arith.h             */

#define NTL_BITS_PER_LONG (64)

#if (!defined(NTL_MAXIMIZE_SP_NBITS) && NTL_BITS_PER_LONG >= 64)
#define NTL_SP_NBITS (NTL_BITS_PER_LONG - 4)
#else
#define NTL_SP_NBITS (NTL_BITS_PER_LONG - 2)
#endif

#if (NTL_BITS_PER_LONG >= NTL_SP_NBITS + 4)

#define NTL_PRE_SHIFT1 (NTL_BITS_PER_LONG - NTL_SP_NBITS - 4)
#define NTL_POST_SHIFT (0)

#define NTL_PRE_SHIFT2 (2 * NTL_SP_NBITS + 2)

#else

// DIRT: This assumes NTL_BITS_PER_LONG == NTL_SP_NBITS+2.
// There are checks in lip.h to verify this.

#define NTL_PRE_SHIFT1 (0)
#define NTL_POST_SHIFT (1)

#define NTL_PRE_SHIFT2 (2 * NTL_SP_NBITS + 1)

#endif

#ifdef USE_STD_INT

#define BUF_SIZE (MRAM_SIZE / 4 / sizeof(int64_t))
#define C_SIZE (WRAM_SIZE) / NR_TASKLETS / 4 / sizeof(int64_t)

__mram_noinit int64_t DPU_ROW_THIS[BUF_SIZE];
__mram_noinit int64_t DPU_ROW_OTHER[BUF_SIZE];
__host uint32_t DPU_SIZE;
__host int64_t DPU_PI;
__host uint64_t DPU_INV;
__host int64_t DPU_SHAMT;


uint64_t ll_get_hi(const uint64_t x)
{
    return ((uint64_t)(x >> NTL_BITS_PER_LONG));
}

uint64_t ll_mul_hi(uint64_t a, uint64_t b)
{
    uint64_t x = a * b;
    return ll_get_hi(x);
}

uint64_t ll_get_lo(const uint64_t x)
{
    return (uint64_t) x;
}

uint64_t cast_unsigned(int64_t a)
{
    return (uint64_t) a;
}

int64_t sp_CorrectExcess(int64_t a, int64_t n)
{
    return a - n >= 0 ? a - n : a;
}

int64_t sp_NormalizedMulMod(int64_t a, int64_t b, int64_t n, uint64_t ninv)
{
    uint64_t U = a * b;
    uint64_t H = U >> (NTL_SP_NBITS - 2);
    uint64_t Q = ll_mul_hi(H, ninv);
    Q = Q >> NTL_POST_SHIFT;
    uint64_t L = ll_get_lo(U);
    int64_t r = L - Q * cast_unsigned(n); // r in [0..2*n)

    r = sp_CorrectExcess(r, n);
    return r;
}

int64_t MulMod(int64_t a, int64_t b, int64_t n, uint64_t inv, int64_t shamt)
{
    return sp_NormalizedMulMod(a, b << shamt, n << shamt, inv) >> shamt;
}

void ctxt_mul(int64_t start, int64_t size)
{
    __dma_aligned int64_t row_this[C_SIZE];
    __dma_aligned int64_t row_other[C_SIZE];

    int64_t _size = size < (C_SIZE) ? size : C_SIZE;
    int64_t cache_copy_size = _size * sizeof(int64_t);

    for (int64_t i = start; i < start + size; i += _size)
    {
        mram_read(&DPU_ROW_THIS[i], row_this, cache_copy_size);
        mram_read(&DPU_ROW_OTHER[i], row_other, cache_copy_size);

        for (int64_t j = 0; j < _size; j++)
        {
            row_this[j] = MulMod(row_this[j], row_other[j], DPU_PI, DPU_INV, DPU_SHAMT);
        }

        mram_write(row_this, &DPU_ROW_THIS[i], cache_copy_size);
    }
}

#else

#define BUF_SIZE (MRAM_SIZE / 4 / sizeof(long))
#define C_SIZE (WRAM_SIZE) / NR_TASKLETS / 4 / sizeof(int64_t)

__mram_noinit long DPU_ROW_THIS[BUF_SIZE];
__mram_noinit long DPU_ROW_OTHER[BUF_SIZE];
__host int DPU_SIZE;
__host long DPU_PI;
__host unsigned long DPU_INV;
__host long DPU_SHAMT;

unsigned long ll_get_hi(const unsigned long long x)
{
    return ((unsigned long) (x >> NTL_BITS_PER_LONG));
}

unsigned long ll_mul_hi(unsigned long a, unsigned long b)
{
    unsigned long long x = ((unsigned long long) a) * ((unsigned long long) b);
    return ll_get_hi(x);
}

unsigned long ll_get_lo(const unsigned long long x)
{
    return ((unsigned long) x);
}

unsigned long cast_unsigned(long a)
{
    return ((unsigned long) a);
}

long sp_CorrectExcess(long a, long n)
{
    return a - n >= 0 ? a - n : a;
}

long sp_NormalizedMulMod(long a, long b, long n, unsigned long ninv)
{
    unsigned long long U = ((unsigned long long) ((long) (a))) * ((unsigned long long) ((long)(b)));
    unsigned long H = ((unsigned long) (U >> (NTL_SP_NBITS - 2)));
    unsigned long Q = ll_mul_hi(H, ninv);
    Q = Q >> NTL_POST_SHIFT;
    unsigned long L = ll_get_lo(U);
    long r = L - Q * cast_unsigned(n); // r in [0..2*n)

    r = sp_CorrectExcess(r, n);
    return r;
}

long MulMod(long a, long b, long n, unsigned long inv, long shamt)
{
    return sp_NormalizedMulMod(a, b << shamt, n << shamt, inv) >> shamt;
}

void ctxt_mul(int64_t start, int64_t size)
{
    __dma_aligned long row_this[C_SIZE];
    __dma_aligned long row_other[C_SIZE];

    int64_t _size = size < (C_SIZE) ? size : C_SIZE;
    int64_t cache_copy_size = _size * sizeof(long);

    for (int64_t i = start; i < start + size; i += _size)
    {
        mram_read(&DPU_ROW_THIS[i], row_this, cache_copy_size);
        mram_read(&DPU_ROW_OTHER[i], row_other, cache_copy_size);

        for (int64_t j = 0; j < _size; j++)
        {
            row_this[j] = MulMod(row_this[j], row_other[j], DPU_PI, DPU_INV, DPU_SHAMT);
        }

        mram_write(row_this, &DPU_ROW_THIS[i], cache_copy_size);
    }
}

#endif

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

    // printf(
    //     "me: %u\n  DPU_SIZE: %u - BUF_SIZE: %u - C_SIZE: %d - pi: %lld\n  size: %u - nr_tasklets %u - DPU_INV: %lu - DPU_SHAMT: %ld\n",
    //     idx, DPU_SIZE, BUF_SIZE, C_SIZE, DPU_PI, DPU_SIZE / nr_tasklets, nr_tasklets, DPU_INV, DPU_SHAMT);
    ctxt_mul(idx * DPU_SIZE / nr_tasklets, DPU_SIZE / nr_tasklets);
    // DPU_ROW_THIS[10] = MulMod(DPU_ROW_THIS[0], DPU_ROW_OTHER[0], DPU_PI, DPU_INV, DPU_SHAMT);

    return 0;
}
