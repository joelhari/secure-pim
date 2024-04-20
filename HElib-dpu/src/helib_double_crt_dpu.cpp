
#include <dpu>
#include <iostream>
#include <helib/log.h>
#include <helib/timing.h>
#include <helib/helib.h>
#include <helib/DoubleCRT.h>
#include "helib_double_crt_dpu.hpp"
#include "common.h"

#define DPU_BINARY_DIR "./build/dpu/"
#define DPU_BINARY_ADD_CTXT DPU_BINARY_DIR "dpu_add_ctxt"
#define DPU_BINARY_MUL_CTXT DPU_BINARY_DIR "dpu_mul_ctxt"
#define DPU_BINARY_TO_POLY DPU_BINARY_DIR "dpu_to_poly"

using namespace helib;

void to_long_array(const NTL::vec_long &from, long* to)
{
    for (long i = 0; i < from.length(); i++)
    {
        to[i] = from[i];
    }
}

void dpu_add_ctxt(
    helib::IndexMap<NTL::vec_long> *this_map,
    const helib::IndexMap<NTL::vec_long> *other_map,
    const helib::Context *context,
    helib::IndexSet s,
    long phim)
{
    std::cout << "s.card(): " << s.card() << std::endl;
    std::cout << "phim: " << phim << std::endl;

    struct dpu_set_t set, dpu;

    uint32_t loops = s.card(); // s.card() is not always a power of 2
    uint32_t size = phim;

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

    // use a power of 2 number of DPUs per loop 
    uint32_t per_loop_dpus = 1;
    while (per_loop_dpus * per_iteration_loops <= NR_DPUS / 2)
    {
        per_loop_dpus *= 2;
    }

    uint32_t nr_dpus = per_loop_dpus * per_iteration_loops;
    if (NR_DPUS % per_iteration_loops != 0)
    {
        Warning("Error in dpu_COBRA_2: per iteration loops " + std::to_string(per_iteration_loops));
    }

    uint32_t per_dpu_size = size / per_loop_dpus;
    uint32_t last_dpu_size = size % per_dpu_size;
    last_dpu_size = last_dpu_size == 0 ? per_dpu_size : last_dpu_size;

    // std::cout << "loops = " << loops << std::endl;
    // std::cout << "size = " << size << std::endl;
    // std::cout << "nr_iterations = " << nr_iterations << std::endl;
    // std::cout << "per_iteration_loops = " << per_iteration_loops << std::endl;
    // std::cout << "per_loop_dpus = " << per_loop_dpus << std::endl;
    // std::cout << "per_dpu_size = " << per_dpu_size << std::endl;
    // std::cout << "last_dpu_size = " << last_dpu_size << std::endl;
    // std::cout << "nr_dpus = " << nr_dpus << std::endl;
    // std::cout << "sizeof(long) = " << sizeof(long) << std::endl;

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_ADD_CTXT, NULL));

    // TODO: implement iterations

    HELIB_NTIMER_START(time_dpu_time_add_ctxt_data_to_dpu);
    struct dpu_set_dpu_iterator_t dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    struct dpu_set_dpu_iterator_t dpu_it_1 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long dpu_idx_0 = 0;
        long dpu_idx_1 = 0;
        long pi = context->ithPrime(i);
        NTL::vec_long &row = (*this_map)[i];
        const NTL::vec_long &other_row = (*other_map)[i];

        // need to copy other_row data to new long array, since
        // `dpu_prepare_xfer` doesn't work with a `const void *` pointer
        long other_row_long[other_row.length()];
        to_long_array(other_row, other_row_long);

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &row[dpu_idx_0 * per_dpu_size]));
            dpu_idx_0++;
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_ROW_THIS), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_1.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &other_row_long[dpu_idx_1 * per_dpu_size]));
            dpu_idx_1++;
            dpu_set_dpu_iterator_next(&dpu_it_1);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_ROW_OTHER), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));
    }
    dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long pi = context->ithPrime(i);
        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_PI), 0, &pi, sizeof(long)));
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
    }
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_add_ctxt_data_to_dpu);

    // std::cout << "dpu launch" << std::endl;
    HELIB_NTIMER_START(time_dpu_time_add_ctxt);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_add_ctxt);

    // DPU_FOREACH(set, dpu)
    // {
    //     DPU_ASSERT(dpu_log_read(dpu, stdout));
    // }

    HELIB_NTIMER_START(time_dpu_time_add_ctxt_data_from_dpu);
    dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long dpu_idx_0 = 0;
        long pi = context->ithPrime(i);
        NTL::vec_long &row = (*this_map)[i];

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &row[dpu_idx_0 * per_dpu_size]));
            dpu_idx_0++;
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, XSTR(DPU_ROW_THIS), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));
    }
    HELIB_NTIMER_STOP(time_dpu_time_add_ctxt_data_from_dpu);

    DPU_ASSERT(dpu_free(set));
}

void dpu_mul_ctxt(
    helib::IndexMap<NTL::vec_long> *this_map,
    const helib::IndexMap<NTL::vec_long> *other_map,
    const helib::Context *context,
    helib::IndexSet s,
    long phim)
{
    std::cout << "s.card(): " << s.card() << std::endl;
    std::cout << "phim: " << phim << std::endl;

    struct dpu_set_t set, dpu;

    uint32_t loops = s.card(); // s.card() is not always a power of 2
    uint32_t size = phim;

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

    // use a power of 2 number of DPUs per loop 
    uint32_t per_loop_dpus = 1;
    while (per_loop_dpus * per_iteration_loops <= NR_DPUS / 2)
    {
        per_loop_dpus *= 2;
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
    std::cout << "sizeof(long) = " << sizeof(long) << std::endl;

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_MUL_CTXT, NULL));

    // TODO: implement iterations

    HELIB_NTIMER_START(time_dpu_time_mul_ctxt_data_to_dpu);
    struct dpu_set_dpu_iterator_t dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    struct dpu_set_dpu_iterator_t dpu_it_1 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long dpu_idx_0 = 0;
        long dpu_idx_1 = 0;
        NTL::vec_long &row = (*this_map)[i];
        const NTL::vec_long &other_row = (*other_map)[i];

        // need to copy other_row data to new long array, since
        // `dpu_prepare_xfer` doesn't work with a `const void *` pointer
        long other_row_long[other_row.length()];
        to_long_array(other_row, other_row_long);

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &row[dpu_idx_0 * per_dpu_size]));
            dpu_idx_0++;
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_ROW_THIS), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_1.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &other_row_long[dpu_idx_1 * per_dpu_size]));
            dpu_idx_1++;
            dpu_set_dpu_iterator_next(&dpu_it_1);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_ROW_OTHER), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));
    }
    dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long pi = context->ithPrime(i);
        NTL::mulmod_t pi_inv = context->ithModulus(i).getQInv();

        std::cout << "inv: " << pi_inv.inv << " - shamt: " << pi_inv.shamt << std::endl;

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_PI), 0, &pi, sizeof(long)));
            DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_INV), 0, &pi_inv.inv, sizeof(unsigned long)));
            DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_SHAMT), 0, &pi_inv.shamt, sizeof(long)));
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
    }
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_mul_ctxt_data_to_dpu);

    std::cout << "dpu launch" << std::endl;
    HELIB_NTIMER_START(time_dpu_time_mul_ctxt);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_mul_ctxt);

    DPU_FOREACH(set, dpu)
    {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }

    HELIB_NTIMER_START(time_dpu_time_mul_ctxt_data_from_dpu);
    dpu_it_0 = dpu_set_dpu_iterator_from(&set);
    for (long i : s)
    {
        long dpu_idx_0 = 0;
        long pi = context->ithPrime(i);
        NTL::vec_long &row = (*this_map)[i];

        for (long j = 0; j < per_loop_dpus; j++)
        {
            dpu = dpu_it_0.next;
            DPU_ASSERT(dpu_prepare_xfer(dpu, &row[dpu_idx_0 * per_dpu_size]));
            dpu_idx_0++;
            dpu_set_dpu_iterator_next(&dpu_it_0);
        }
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, XSTR(DPU_ROW_THIS), 0, sizeof(long) * per_dpu_size, DPU_XFER_DEFAULT));
    }
    HELIB_NTIMER_STOP(time_dpu_time_mul_ctxt_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    /* for (long i : s) {
        long pi = context->ithPrime(i);
        NTL::vec_long& row = (*this_map)[i];
        const NTL::vec_long& other_row = (*other_map)[i];

        NTL::mulmod_t pi_inv = context->ithModulus(i).getQInv();
        for (long j : range(phim))
        {
            row[j] = MulMod(row[j], other_row[j], pi, pi_inv);
        }
    } */
}

void dpu_toPoly(NTL::ZZX &poly, const helib::IndexSet &s, bool positive)
{
    std::cout << "dpu_toPoly" << std::endl;

    // initialize the ivec vector, ivec[j] = index of j'th active prime
    // long phim = context.getPhiM();
    // long icard = MakeIndexVector(s1, ivec); // icard = how many active primes
}
