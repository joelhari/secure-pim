
#include <dpu>
#include <iostream>
#include <helib/log.h>
#include <helib/timing.h>
#include "helib_c_modulus_dpu.hpp"
#include "common.h"

#define DPU_BINARY_DIR "./build/dpu/"
#define DPU_BINARY_COBRA DPU_BINARY_DIR "dpu_cobra"
#define DPU_BINARY_BASIC_BIT_REVERSE_COPY DPU_BINARY_DIR "dpu_basic_bit_reverse_copy"

#define D_CACHE_SIZE WRAM_SIZE / sizeof(int64_t) / 4

// TODO: find a better way to get the max number of allocatable dpus
#define MAX_NR_DPUS 64
#define GET_MAX_NR_DPUS(max_nr_dpus)                         \
    {                                                        \
        struct dpu_set_t set;                                \
        DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &set)); \
        DPU_ASSERT(dpu_get_nr_dpus(set, &max_nr_dpus));      \
        DPU_ASSERT(dpu_free(set));                           \
    }

#ifndef NR_DPUS
#define NR_DPUS MAX_NR_DPUS
#endif

using namespace helib;

void dpu_BasicBitReverseCopy(
    long *B,
    const long *A,
    long k,
    long *rev)
{
    long n = 1L << k;

    // for (long i = 0; i < n; i++)
    // {
    //     std::cout << "rev[" << i << "] = " << rev[i] << std::endl;
    // }

    // std::cout << "A[0] = " << A[0] << std::endl;
    // std::cout << "sizeof(long) " << sizeof(long) << std::endl;
    // std::cout << "sizeof(int64_t) " << sizeof(int64_t) << std::endl;

    struct dpu_set_t set, dpu;
    uint32_t per_dpu_size, last_dpu_size, each_dpu;
    // uint32_t nr_dpus = NR_DPUS;
    // long dpu_start;

    // per_dpu_size = n / nr_dpus;
    // last_dpu_size = n % nr_dpus;

    // DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &set));
    // DPU_ASSERT(dpu_load(set, DPU_BINARY_BASIC_BIT_REVERSE_COPY, NULL));

    // HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy_data_to_dpu);
    // DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_A), 0, A, n * sizeof(long), DPU_XFER_DEFAULT));
    // DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_REV_Q), 0, rev, n * sizeof(long), DPU_XFER_DEFAULT));
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     if (nr_dpus - 1 == each_dpu)
    //     {
    //         DPU_ASSERT(dpu_prepare_xfer(dpu, &last_dpu_size));
    //     }
    //     else
    //     {
    //         DPU_ASSERT(dpu_prepare_xfer(dpu, &per_dpu_size));
    //     }
    // }
    // DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_SIZE), 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     dpu_start = each_dpu * per_dpu_size;
    //     DPU_ASSERT(dpu_broadcast_to(dpu, XSTR(DPU_START), 0, &dpu_start, sizeof(long), DPU_XFER_DEFAULT));
    // }
    // HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy_data_to_dpu);

    // HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy);
    // DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    // HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy);

    // HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy_data_from_dpu);
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     for (long i = 0; i < (nr_dpus - 1 == each_dpu ? last_dpu_size : per_dpu_size); i++)
    //     {
    //         DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_B), rev[i + each_dpu * per_dpu_size], &B[]))
    //     }
    // }
    // HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy_data_from_dpu);

    // std::cout << "n " << n << std::endl;
    // std::cout << "sizeof(int64_t) " << sizeof(int64_t) << std::endl;
    // std::cout << "D_CACHE_SIZE " << D_CACHE_SIZE << std::endl;
    // std::cout << "WRAM_SIZE " << WRAM_SIZE << std::endl;

    if (n > D_CACHE_SIZE)
    {
        throw DPU_ERR_ALLOCATION;
    }

    DPU_ASSERT(dpu_alloc(1, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_BASIC_BIT_REVERSE_COPY, NULL));

    HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy_data_to_dpu);
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_A), 0, A, n * sizeof(long), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_REV_Q), 0, rev, n * sizeof(long), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &n, sizeof(long), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy_data_to_dpu);

    HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy);

    // DPU_FOREACH(set, dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }

    HELIB_NTIMER_START(time_dpu_time_basic_bit_reverse_copy_data_from_dpu);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_B), 0, B, n * sizeof(long)));
    }
    HELIB_NTIMER_STOP(time_dpu_time_basic_bit_reverse_copy_data_from_dpu);

    DPU_ASSERT(dpu_free(set));
}

void dpu_COBRA(
    long *B,
    const long *A,
    long k1,
    long *rev_k1,
    long *rev_q,
    long q)
{
    long a1, b1, c1;

    struct dpu_set_t set, dpu;
    uint32_t per_dpu_size, each_dpu;

    uint32_t nr_dpus = 1L << k1;

    DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_COBRA, NULL));

    HELIB_NTIMER_START(time_dpu_time_cobra_data_to_dpu);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        for (long a = 0; a < (1L << q); a++)
        {
            a1 = rev_q[a];
            DPU_ASSERT(dpu_broadcast_to(
                dpu,
                XSTR(DPU_A),
                a * (1L << q) * sizeof(long),
                &A[(a << (k1 + q)) + (each_dpu << q)],
                (1L << q) * sizeof(long),
                DPU_XFER_DEFAULT));
        }
    }

    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_REV_Q), 0, &rev_q[0], (1L << q) * sizeof(long), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_Q), 0, &q, sizeof(long), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_cobra_data_to_dpu);

    HELIB_NTIMER_START(time_dpu_time_cobra);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_cobra);

    // DPU_FOREACH(set, dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }

    HELIB_NTIMER_START(time_dpu_time_cobra_data_from_dpu);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        b1 = rev_k1[each_dpu];

        for (long c = 0; c < (1L << q); c++)
        {
            c1 = rev_q[c];
            DPU_ASSERT(dpu_copy_from(
                dpu,
                XSTR(DPU_B),
                c * (1L << q) * sizeof(long),
                &B[(c1 << (k1 + q)) + (b1 << q)],
                (1L << q) * sizeof(long)));
        }
    }
    HELIB_NTIMER_STOP(time_dpu_time_cobra_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // for (long b = 0; b < (1L << k1); b++)
    // {
    //     for (long a = 0; a < (1L << q); a++)
    //     {
    //         a1 = rev_q[a];
    //         for (long c = 0; c < (1L << q); c++)
    //             dpu_A[a * (1L << q) + c] = A[(a << (k1 + q)) + (b << q) + c];
    //     }

    //     for (long a = 0; a < (1L << q); a++) {
    //         a1 = rev_q[a];
    //         for (long c = 0; c < (1L << q); c++)
    //             dpu_T[(a1 << q) + c] = dpu_A[(a * (1L << q)) + c];
    //     }

    //     for (long c = 0; c < (1L << q); c++) {
    //         c1 = rev_q[c];
    //         for (long a1 = 0; a1 < (1L << q); a1++)
    //             dpu_B[(c * (1L << q)) + a1] = dpu_T[(a1 << q) + c];
    //     }

    //     b1 = rev_k1[b];

    //     for (long c = 0; c < (1L << q); c++)
    //     {
    //         c1 = rev_q[c];
    //         for (long a1 = 0; a1 < (1L << q); a1++)
    //             B[(c1 << (k1 + q)) + (b1 << q) + a1] = dpu_B[c * (1L << q) + a1];
    //     }
    // }
}

void dpu_BitReverseCopy(long *NTL_RESTRICT B, const long *NTL_RESTRICT A, long k)
{
    std::cout << "dpu_BitReverseCopy" << std::endl;
}
