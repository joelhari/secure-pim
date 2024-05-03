
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

/**
 * DPU host application method for the implementation of CModulus::COBRA method.
 * Makes use of `1L << k1` DPUs, where each DPU is responsible for one loop of
 * the `for (long b = 0; b < (1L << k1); b++) { ... }` for loop from the original
 * method.
 */
void dpu_COBRA_1(
    long *B,
    const long *A,
    long k1,
    long *rev_k1,
    long *rev_q,
    long q)
{
    long a1, b1, c1;

    struct dpu_set_t set, dpu;
    uint32_t each_dpu;

    uint32_t nr_dpus = 1L << k1;
    while (nr_dpus > NR_DPUS) {
        nr_dpus /= 2;
    }

    uint32_t loops = 1L << k1;
    uint32_t nr_iterations = 1;
    while (loops / nr_iterations > nr_dpus)
    {
        nr_iterations *= 2;
    }

    std::cout << "nr_dpus = " << nr_dpus << std::endl;

    DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_COBRA, NULL));

    for (long i = 0; i < nr_iterations; i++)
    {
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
                    &A[(a << (k1 + q)) + ((each_dpu + i*nr_dpus) << q)],
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
            b1 = rev_k1[(each_dpu + i*nr_dpus)];

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
    };

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

/**
 * !! This method does not work. !!
 * In this method I tried to make use of more DPUs.
 * However, the copying of the values from A to B does not happen in any
 * grouped way, i.e. no two values who are next to each other in A will be
 * next to each other in B.
 * Thus, splitting up the inner loops over multiple DPUs is useless, since in 
 * the end we would have to stitch the resulting array together by copying
 * single values from the DPUs to the correct place in the array.
 */
void dpu_COBRA_2(
    long *B,
    const long *A,
    long k1,
    long *rev_k1,
    long *rev_q,
    long q)
{
    long a1, b1, c1;
    long offset = 0L;

    struct dpu_set_t set, dpu;

    std::vector<dpu_set_t> _dpus;

    uint32_t loops = 1L << k1;
    uint32_t size = 1L << q;
    uint32_t dpus_per_loop = NR_DPUS / loops;
    if (dpus_per_loop == 0) {

    }

    uint32_t nr_iterations = 1;
    while (loops / nr_iterations > NR_DPUS)
    {
        nr_iterations *= 2;
    }

    if (loops % nr_iterations != 0)
    {
        Warning("Error in dpu_COBRA_2: nr of loops " + std::to_string(loops) + " is not divisible by nr of iterations " + std::to_string(nr_iterations));
    }

    uint32_t per_iteration_loops = loops / nr_iterations;
    uint32_t per_loop_dpus = NR_DPUS / per_iteration_loops;
    while (size / per_loop_dpus < 1)
    {
        per_loop_dpus /= 2;
    }
    uint32_t nr_dpus = per_loop_dpus * per_iteration_loops;

    if (NR_DPUS % per_iteration_loops != 0)
    {
        Warning("Error in dpu_COBRA_2: per iteration loops " + std::to_string(per_iteration_loops));
    }

    uint32_t per_dpu_size = size / per_loop_dpus;
    uint32_t last_dpu_size = size % per_dpu_size;
    last_dpu_size = last_dpu_size == 0 ? per_dpu_size : last_dpu_size;

    std::cout << "loops = " << loops << std::endl;
    std::cout << "size = " << size << std::endl;
    std::cout << "nr_iterations = " << nr_iterations << std::endl;
    std::cout << "per_iteration_loops = " << per_iteration_loops << std::endl;
    std::cout << "per_loop_dpus = " << per_loop_dpus << std::endl;
    std::cout << "per_dpu_size = " << per_dpu_size << std::endl;
    std::cout << "last_dpu_size = " << last_dpu_size << std::endl;
    std::cout << "nr_dpus = " << nr_dpus << std::endl;

    DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_COBRA, NULL));

    struct dpu_set_t cRank;
    DPU_RANK_FOREACH (set, cRank) {
        struct dpu_set_t cDpu;
        DPU_FOREACH (cRank, cDpu) {
            _dpus.push_back(cDpu);
        }
    }

    do {
        HELIB_NTIMER_START(time_dpu_time_cobra_data_to_dpu);
        for (long b = 0; b < (1L << k1); b++)
        {
            for (long a = 0; a < (1L << q); a++)
            {
                a1 = rev_q[a];
                dpu = _dpus.at(b * per_loop_dpus + a1 % per_loop_dpus);
                DPU_ASSERT(dpu_broadcast_to(
                    dpu,
                    XSTR(DPU_A),
                    a * (1L << q) * sizeof(long),
                    &A[(a << (k1 + q)) + (b << q)],
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
        for (long b = 0; b < (1L << k1); b++)
        {
            b1 = rev_k1[b];

            for (long c = 0; c < (1L << q); c++)
            {
                c1 = rev_q[c];
                dpu = _dpus.at(b * per_loop_dpus + a1 % per_loop_dpus);
                DPU_ASSERT(dpu_copy_from(
                    dpu,
                    XSTR(DPU_B),
                    c * (1L << q) * sizeof(long),
                    &B[(c1 << (k1 + q)) + (b1 << q)],
                    (1L << q) * sizeof(long)));
            }
        }
        HELIB_NTIMER_STOP(time_dpu_time_cobra_data_from_dpu);
    } while (--nr_iterations != 0);

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
