
#include <dpu>
#include <iostream>
#include <helib/log.h>
#include <helib/timing.h>
#include "complex_conversions.hpp"
#include "helib_pgfft_dpu.hpp"
#include "common.h"

#define FFT 0
#define IFFT 1

#define DPU_BINARY_DIR "./build/dpu/"
#define DPU_BINARY_HELLO_WORLD DPU_BINARY_DIR "dpu_hello_world"
#define DPU_BINARY_MUL DPU_BINARY_DIR "dpu_mul"
#define DPU_BINARY_FWD_BUTTERFLY_LOOP DPU_BINARY_DIR "dpu_fwd_butterfly_loop"
#define DPU_BINARY_MULTI_FWD_BUTTERFLY_LOOP DPU_BINARY_DIR "dpu_multi_fwd_butterfly_loop"
#define DPU_BINARY_INV_BUTTERFLY_LOOP DPU_BINARY_DIR "dpu_inv_butterfly_loop"
#define DPU_BINARY_MULTI_INV_BUTTERFLY_LOOP DPU_BINARY_DIR "dpu_multi_inv_butterfly_loop"
#define DPU_BINARY_NEW_FFT_LAST_TWO_LAYERS DPU_BINARY_DIR "dpu_new_fft_last_two_layers"
#define DPU_BINARY_MULTI_NEW_FFT_LAST_TWO_LAYERS DPU_BINARY_DIR "dpu_multi_new_fft_last_two_layers"
#define DPU_BINARY_NEW_IFFT_FIRST_TWO_LAYERS DPU_BINARY_DIR "dpu_new_ifft_first_two_layers"
#define DPU_BINARY_MULTI_NEW_IFFT_FIRST_TWO_LAYERS DPU_BINARY_DIR "dpu_multi_new_ifft_first_two_layers"

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

#define GET_NR_DPUS_AND_SIZE_PER_DPU(size, nr_dpus, per_dpu_size, last_dpu_size) \
    {                                                                            \
        per_dpu_size = 4;                                                        \
        nr_dpus = ceil(size / (float)per_dpu_size);                              \
        while (nr_dpus > NR_DPUS)                                                \
        {                                                                        \
            per_dpu_size *= 2;                                                   \
            nr_dpus = ceil(size / (float)per_dpu_size);                          \
        }                                                                        \
        last_dpu_size = size % per_dpu_size;                                     \
        last_dpu_size = last_dpu_size == 0 ? per_dpu_size : last_dpu_size;       \
    }

#define GET_NR_DPUS_AND_BLOCKS_PER_DPU(blocks, nr_dpus, per_dpu_blocks, last_dpu_blocks) \
    {                                                                                    \
        per_dpu_blocks = 1;                                                              \
        nr_dpus = ceil(blocks / (float)per_dpu_blocks);                                  \
        while (nr_dpus > NR_DPUS)                                                        \
        {                                                                                \
            per_dpu_blocks *= 2;                                                         \
            nr_dpus = ceil(blocks / (float)per_dpu_blocks);                              \
        }                                                                                \
        last_dpu_blocks = blocks % per_dpu_blocks;                                       \
        last_dpu_blocks = last_dpu_blocks == 0 ? per_dpu_blocks : last_dpu_blocks;       \
    }

using namespace dpu;
using namespace helib;

void dpu_hello_world()
{
    struct dpu_set_t set, dpu;
    uint32_t nr_dpus;

    DPU_ASSERT(dpu_alloc(4, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_HELLO_WORLD, NULL));
    DPU_ASSERT(dpu_get_nr_dpus(set, &nr_dpus));

    HELIB_NTIMER_START(time_dpu_time_hello_world);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_hello_world);

    DPU_FOREACH(set, dpu)
    {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }

    DPU_ASSERT(dpu_free(set));
}

std::complex<double> dpu_mul(std::complex<double> a, std::complex<double> b)
{
    struct dpu_set_t set, dpu;

    DPU_ASSERT(dpu_alloc(1, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_MUL, NULL));

    dpu_complex dpu_a = convert_to_dpu_complex(a);
    dpu_complex dpu_b = convert_to_dpu_complex(b);

    DPU_ASSERT(dpu_broadcast_to(set, "a", 0, &dpu_a, sizeof(dpu_complex), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "b", 0, &dpu_b, sizeof(dpu_complex), DPU_XFER_DEFAULT));

    HELIB_NTIMER_START(time_dpu_time_mul);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_mul);

    DPU_FOREACH(set, dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_a));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "a", 0, sizeof(dpu_complex), DPU_XFER_DEFAULT));

    DPU_ASSERT(dpu_free(set));

    return std::complex<double>(dpu_a.real, dpu_a.imag);
}

void dpu_transfer_complex_array_to_dpu(
    dpu_set_t set,
    uint32_t per_dpu_size,
    uint32_t last_dpu_size,
    const char *symbol_name,
    dpu_complex *dpu_array)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    uint32_t nr_dpus;
    dpu_get_nr_dpus(set, &nr_dpus);

    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (nr_dpus - 1 == each_dpu)
        {
            DPU_ASSERT(dpu_broadcast_to(dpu, symbol_name, 0, &dpu_array[each_dpu * per_dpu_size], sizeof(dpu_complex) * last_dpu_size, DPU_XFER_DEFAULT));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_array[each_dpu * per_dpu_size]));
        }
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, symbol_name, 0, sizeof(dpu_complex) * per_dpu_size, DPU_XFER_DEFAULT));
}

void dpu_transfer_complex_array_from_dpu(
    dpu_set_t set,
    uint32_t per_dpu_size,
    uint32_t last_dpu_size,
    const char *symbol_name,
    dpu_complex *dpu_array)
{
    struct dpu_set_t dpu;
    uint32_t each_dpu;
    uint32_t nr_dpus;
    dpu_get_nr_dpus(set, &nr_dpus);

    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (nr_dpus - 1 == each_dpu)
        {
            DPU_ASSERT(dpu_copy_from(dpu, symbol_name, 0, &dpu_array[each_dpu * per_dpu_size], sizeof(dpu_complex) * last_dpu_size));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_array[each_dpu * per_dpu_size]));
        }
    }
    if (nr_dpus > 1)
    {
        DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, symbol_name, 0, sizeof(dpu_complex) * per_dpu_size, DPU_XFER_DEFAULT));
    }
}

void dpu_transfer_complex_array_to_dpu(
    dpu_set_t set,
    dpu_set_dpu_iterator_t *dpu_it,
    uint32_t per_block_dpus,
    uint32_t per_dpu_size,
    const char *symbol_name,
    dpu_complex *dpu_array)
{
    struct dpu_set_t dpu;
    for (uint32_t j = 0; j < per_block_dpus; j++)
    {
        dpu = dpu_it->next;
        DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_array[j * per_dpu_size]));
        dpu_set_dpu_iterator_next(dpu_it);
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, symbol_name, 0, sizeof(dpu_complex) * per_dpu_size, DPU_XFER_DEFAULT));
}

void dpu_transfer_complex_array_from_dpu(
    dpu_set_t set,
    dpu_set_dpu_iterator_t *dpu_it,
    uint32_t per_block_dpus,
    uint32_t per_dpu_size,
    const char *symbol_name,
    dpu_complex *dpu_array)
{
    struct dpu_set_t dpu;
    for (uint32_t j = 0; j < per_block_dpus; j++)
    {
        dpu = dpu_it->next;
        DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_array[j * per_dpu_size]));
        dpu_set_dpu_iterator_next(dpu_it);
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, symbol_name, 0, sizeof(dpu_complex) * per_dpu_size, DPU_XFER_DEFAULT));
}

void dpu_transfer_size_to_dpu(dpu_set_t set, uint32_t nr_dpus, uint32_t per_dpu_size, uint32_t last_dpu_size, const char *symbol_name)
{
    dpu_set_t dpu;
    uint32_t each_dpu;

    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (nr_dpus - 1 == each_dpu)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &last_dpu_size));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &per_dpu_size));
        }
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, symbol_name, 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
}

void dpu_transfer_do_butterfly0_to_dpu(dpu_set_t set, const char *symbol_name)
{
    dpu_set_t dpu;
    uint32_t each_dpu;
    uint32_t do_butterfly0_true = 1;
    uint32_t do_butterfly0_false = 0;

    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (each_dpu == 0)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &do_butterfly0_true));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &do_butterfly0_false));
        }
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, symbol_name, 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
}

void dpu_transfer_do_butterfly0_to_dpu(dpu_set_t set, dpu_set_dpu_iterator_t *dpu_it, const char *symbol_name, uint32_t per_block_dpus)
{
    dpu_set_t dpu;
    uint32_t do_butterfly0_true = 1;
    uint32_t do_butterfly0_false = 0;

    for (uint32_t j = 0; j < per_block_dpus; j++)
    {
        dpu = dpu_it->next;
        if (j == 0)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &do_butterfly0_true));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &do_butterfly0_false));
        }
        dpu_set_dpu_iterator_next(dpu_it);
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, symbol_name, 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
}

void dpu_new_fft_ifft_layer(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab, uint8_t fft_ifft)
{
    struct dpu_set_t set, dpu, sub_set;
    uint32_t per_dpu_size;

    uint32_t nr_iterations = 1;
    while (blocks / nr_iterations > NR_DPUS)
    {
        nr_iterations *= 2;
    }

    if (blocks % nr_iterations != 0)
    {
        Warning("Error in dpu_new_fft_layer: nr of blocks " + std::to_string(blocks) + " is not divisible by nr of iterations " + std::to_string(nr_iterations));
    }

    uint32_t per_iteration_blocks = blocks / nr_iterations;
    uint32_t per_block_dpus = (float)NR_DPUS / per_iteration_blocks;
    while ((size / 2) / per_block_dpus < 4)
    {
        per_block_dpus /= 2;
    }
    uint32_t nr_dpus = per_block_dpus * per_iteration_blocks;

    if (NR_DPUS % per_iteration_blocks != 0)
    {
        Warning("Error in dpu_new_fft_layer: per iteration blocks " + std::to_string(per_iteration_blocks));
    }

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));

    if (fft_ifft == FFT)
    {
        DPU_ASSERT(dpu_load(set, DPU_BINARY_FWD_BUTTERFLY_LOOP, NULL));
    }
    else if (fft_ifft == IFFT)
    {
        DPU_ASSERT(dpu_load(set, DPU_BINARY_INV_BUTTERFLY_LOOP, NULL));
    }

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp = new dpu_complex[blocks * size];
    dpu_complex *dpu_xp_start = dpu_xp;
    dpu_complex *dpu_xp_start_it = dpu_xp;
    convert_to_dpu_complex(xp, blocks * size, dpu_xp);
    dpu_complex *dpu_wtab = new dpu_complex[blocks * size];
    convert_to_dpu_complex(wtab, blocks * size, dpu_wtab);

    size /= 2;

    per_dpu_size = size / per_block_dpus;
    if (size % per_dpu_size != 0)
    {
        Warning("Error in dpu_new_fft_layer: last dpu size");
    }

    do
    {
        /* ---------------------------------------------------------------------------------------------------------- */
        /* transfer data to DPUs                                                                                      */
        /*     The following if-else-statement is only used because we want to measure the time it takes to tranfer   */
        /*     data to/from the DPUs separately for the fft and ifft method.                                          */
        /*     All differences are marked with comments (e.g. DIFF: ...).                                             */
        if (fft_ifft == FFT)
        {
            HELIB_NTIMER_START(time_dpu_time_new_fft_layer_data_to_dpu); // DIFF: use fft timer

            dpu_xp_start_it = dpu_xp;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);

            for (uint32_t i = 0; i < per_iteration_blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_to_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_to_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);
                dpu_transfer_complex_array_to_dpu(set, &dpu_wtab_it, per_block_dpus, per_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
                dpu_transfer_do_butterfly0_to_dpu(set, &dpu_it, XSTR(DPU_DO_FWD_BUTTERFLY0), per_block_dpus); // DIFF: use DPU_DO_FWD_BUTTERFLY0

                dpu_xp += 2 * size;
            }

            DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
            
            HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer_data_to_dpu); // DIFF: use fft timer
        }
        else
        {
            HELIB_NTIMER_START(time_dpu_time_new_ifft_layer_data_to_dpu); // DIFF: use ifft timer

            dpu_xp_start_it = dpu_xp;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);

            for (uint32_t i = 0; i < per_iteration_blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_to_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_to_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);
                dpu_transfer_complex_array_to_dpu(set, &dpu_wtab_it, per_block_dpus, per_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
                dpu_transfer_do_butterfly0_to_dpu(set, &dpu_it, XSTR(DPU_DO_INV_BUTTERFLY0), per_block_dpus); // DIFF: use DPU_DO_INV_BUTTERFLY0

                dpu_xp += 2 * size;
            }

            DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
            
            HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer_data_to_dpu); // DIFF: use ifft timer
        }
        /* ---------------------------------------------------------------------------------------------------------- */

        // launch DPU program
        if (fft_ifft == FFT)
        {
            HELIB_NTIMER_START(time_dpu_time_new_fft_layer);
            DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
            HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer);
        }
        else if (fft_ifft == IFFT)
        {
            HELIB_NTIMER_START(time_dpu_time_new_ifft_layer);
            DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
            HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer);
        }

        /* ---------------------------------------------------------------------------------------------------------- */
        /* copy data back from DPUs                                                                                   */
        /*     The following if-else-statement is only used because we want to measure the time it takes to tranfer   */
        /*     data to/from the DPUs separately for the fft and ifft method.                                          */
        /*     All differences are marked with comments (e.g. DIFF: ...).                                             */
        if (fft_ifft == FFT)
        {
            HELIB_NTIMER_START(time_dpu_time_new_fft_layer_data_from_dpu); // DIFF: use fft timer
            dpu_xp = dpu_xp_start_it;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);

            for (uint32_t i = 0; i < per_iteration_blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_from_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_from_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);

                dpu_xp += 2 * size;
            }
            HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer_data_from_dpu); // DIFF: use fft timer
        }
        else if (fft_ifft == IFFT)
        {
            HELIB_NTIMER_START(time_dpu_time_new_ifft_layer_data_from_dpu); // DIFF: use ifft timer
            dpu_xp = dpu_xp_start_it;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);

            for (uint32_t i = 0; i < per_iteration_blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_from_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_from_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);

                dpu_xp += 2 * size;
            }
            HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer_data_from_dpu); // DIFF: use ifft timer
        }
        /* ---------------------------------------------------------------------------------------------------------- */

    } while (--nr_iterations != 0);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex array to std::complex array
    convert_from_dpu_complex(dpu_xp_start, blocks * size * 2, xp);
}

void dpu_new_fft_layer(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();
    dpu_new_fft_ifft_layer(xp, blocks, size, wtab, FFT);
}

void dpu_new_ifft_layer(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();
    dpu_new_fft_ifft_layer(xp, blocks, size, wtab, IFFT);
}

void dpu_new_fft_ifft_layer_2(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab, uint8_t fft_ifft)
{
    struct dpu_set_t set, dpu, sub_set;
    uint32_t each_dpu;
    
    // min. size per DPU is 8 => we can use a maximum of "(size / 8) * blocks" DPUs
    uint32_t nr_dpus = NR_DPUS;
    while (nr_dpus > blocks * size / 8)
    {
        nr_dpus /= 2;
    }

    uint32_t per_dpu_blocks = blocks / nr_dpus;
    uint32_t per_block_dpus = nr_dpus / blocks;

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));

    if (fft_ifft == FFT)
    {
        DPU_ASSERT(dpu_load(set, DPU_BINARY_MULTI_FWD_BUTTERFLY_LOOP, NULL));
    }
    else if (fft_ifft == IFFT)
    {
        DPU_ASSERT(dpu_load(set, DPU_BINARY_MULTI_INV_BUTTERFLY_LOOP, NULL));
    }

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp = new dpu_complex[blocks * size];
    dpu_complex *dpu_xp_start = dpu_xp;
    dpu_complex *dpu_xp_start_it = dpu_xp;
    convert_to_dpu_complex(xp, blocks * size, dpu_xp);
    dpu_complex *dpu_wtab = new dpu_complex[blocks * size];
    convert_to_dpu_complex(wtab, blocks * size, dpu_wtab);

    size /= 2;

    uint32_t per_dpu_size = size / (per_block_dpus > 0 ? per_block_dpus : 1);
    if (size % per_dpu_size != 0)
    {
        Warning("Error in dpu_new_fft_layer: last dpu size");
    }

    /* ---------------------------------------------------------------------------------------------------------- */
    /* transfer data to DPUs                                                                                      */
    /*     The following if-else-statement is only used because we want to measure the time it takes to tranfer   */
    /*     data to/from the DPUs separately for the fft and ifft method.                                          */
    /*     All differences are marked with comments (e.g. DIFF: ...).                                             */
    if (fft_ifft == FFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_fft_layer_data_to_dpu); // DIFF: use fft timer
        dpu_xp_start_it = dpu_xp;
        
        if (per_block_dpus > 0) // case: multiple DPUs per block
        {
            per_dpu_blocks = 1;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);
            
            for (uint32_t i = 0; i < blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_to_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_to_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);
                dpu_transfer_complex_array_to_dpu(set, &dpu_wtab_it, per_block_dpus, per_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
                dpu_transfer_do_butterfly0_to_dpu(set, &dpu_it, XSTR(DPU_DO_FWD_BUTTERFLY0), per_block_dpus); // DIFF: use DPU_DO_FWD_BUTTERFLY0

                dpu_xp += 2 * size;
            }
        }
        else // case: multiple blocks per DPU
        {
            DPU_FOREACH(set, dpu, each_dpu)
            {
                for (uint32_t j = 0; j < per_dpu_blocks; j++)
                {
                    dpu_complex *dpu_xp0 = dpu_xp;
                    dpu_complex *dpu_xp1 = dpu_xp + size;
    
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_XP0), j * sizeof(dpu_complex) * per_dpu_size, &dpu_xp0[0], sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_XP1), j * sizeof(dpu_complex) * per_dpu_size, &dpu_xp1[0], sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_WTAB), j * sizeof(dpu_complex) * per_dpu_size, &dpu_wtab[0], sizeof(dpu_complex) * per_dpu_size));

                    dpu_xp += 2 * size;
                }

            }
        }
        DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_BLOCKS), 0, &per_dpu_blocks, sizeof(uint32_t), DPU_XFER_DEFAULT));
        DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
        HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer_data_to_dpu); // DIFF: use fft timer
    }
    else if (fft_ifft == IFFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_ifft_layer_data_to_dpu); // DIFF: use ifft timer
        dpu_xp_start_it = dpu_xp;
        
        if (per_block_dpus > 0) // case: multiple DPUs per block
        {
            per_dpu_blocks = 1;
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);
            
            for (uint32_t i = 0; i < blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_to_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_to_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);
                dpu_transfer_complex_array_to_dpu(set, &dpu_wtab_it, per_block_dpus, per_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
                dpu_transfer_do_butterfly0_to_dpu(set, &dpu_it, XSTR(DPU_DO_INV_BUTTERFLY0), per_block_dpus); // DIFF: use DPU_DO_INV_BUTTERFLY0

                dpu_xp += 2 * size;
            }
        }
        else // case: multiple blocks per DPU
        {
            DPU_FOREACH(set, dpu, each_dpu)
            {
                for (uint32_t j = 0; j < per_dpu_blocks; j++)
                {
                    dpu_complex *dpu_xp0 = dpu_xp;
                    dpu_complex *dpu_xp1 = dpu_xp + size;
    
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_XP0), j * sizeof(dpu_complex) * per_dpu_size, &dpu_xp0[0], sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_XP1), j * sizeof(dpu_complex) * per_dpu_size, &dpu_xp1[0], sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_to(dpu, XSTR(DPU_WTAB), j * sizeof(dpu_complex) * per_dpu_size, &dpu_wtab[0], sizeof(dpu_complex) * per_dpu_size));

                    dpu_xp += 2 * size;
                }

            }
        }
        DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_BLOCKS), 0, &per_dpu_blocks, sizeof(uint32_t), DPU_XFER_DEFAULT));
        DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_SIZE), 0, &per_dpu_size, sizeof(uint32_t), DPU_XFER_DEFAULT));
        HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer_data_to_dpu); // DIFF: use ifft timer
    }
    /* ---------------------------------------------------------------------------------------------------------- */

    // launch DPU program
    if (fft_ifft == FFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_fft_layer);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
        HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer);
    }
    else if (fft_ifft == IFFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_ifft_layer);
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
        HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer);
    }

    /* ---------------------------------------------------------------------------------------------------------- */
    /* copy data back from DPUs                                                                                   */
    /*     The following if-else-statement is only used because we want to measure the time it takes to tranfer   */
    /*     data to/from the DPUs separately for the fft and ifft method.                                          */
    /*     All differences are marked with comments (e.g. DIFF: ...).                                             */
    if (fft_ifft == FFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_fft_layer_data_from_dpu); // DIFF: use fft timer
        dpu_xp = dpu_xp_start_it;

        if (per_block_dpus > 0) // case: multiple DPUs per block
        {
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);
            
            for (uint32_t i = 0; i < blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_from_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_from_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);

                dpu_xp += 2 * size;
            }
        }
        else // case: multiple blocks per DPU
        {
            DPU_FOREACH(set, dpu, each_dpu)
            {
                for (uint32_t j = 0; j < per_dpu_blocks; j++)
                {
                    dpu_complex *dpu_xp0 = dpu_xp;
                    dpu_complex *dpu_xp1 = dpu_xp + size;

                    DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_XP0), j * sizeof(dpu_complex) * per_dpu_size, dpu_xp0, sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_XP1), j * sizeof(dpu_complex) * per_dpu_size, dpu_xp1, sizeof(dpu_complex) * per_dpu_size));

                    dpu_xp += 2 * size;
                }
            }
        }
        HELIB_NTIMER_STOP(time_dpu_time_new_fft_layer_data_from_dpu); // DIFF: use fft timer
    }
    else if (fft_ifft == IFFT)
    {
        HELIB_NTIMER_START(time_dpu_time_new_ifft_layer_data_from_dpu); // DIFF: use ifft timer
        dpu_xp = dpu_xp_start_it;

        if (per_block_dpus > 0) // case: multiple DPUs per block
        {
            struct dpu_set_dpu_iterator_t dpu_xp0_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_xp1_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_wtab_it = dpu_set_dpu_iterator_from(&set);
            struct dpu_set_dpu_iterator_t dpu_it = dpu_set_dpu_iterator_from(&set);
            
            for (uint32_t i = 0; i < blocks; i++)
            {
                dpu_complex *dpu_xp0 = dpu_xp;
                dpu_complex *dpu_xp1 = dpu_xp + size;

                dpu_transfer_complex_array_from_dpu(set, &dpu_xp0_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP0), dpu_xp0);
                dpu_transfer_complex_array_from_dpu(set, &dpu_xp1_it, per_block_dpus, per_dpu_size, XSTR(DPU_XP1), dpu_xp1);

                dpu_xp += 2 * size;
            }
        }
        else // case: multiple blocks per DPU
        {
            DPU_FOREACH(set, dpu, each_dpu)
            {
                for (uint32_t j = 0; j < per_dpu_blocks; j++)
                {
                    dpu_complex *dpu_xp0 = dpu_xp;
                    dpu_complex *dpu_xp1 = dpu_xp + size;

                    DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_XP0), j * sizeof(dpu_complex) * per_dpu_size, dpu_xp0, sizeof(dpu_complex) * per_dpu_size));
                    DPU_ASSERT(dpu_copy_from(dpu, XSTR(DPU_XP1), j * sizeof(dpu_complex) * per_dpu_size, dpu_xp1, sizeof(dpu_complex) * per_dpu_size));

                    dpu_xp += 2 * size;
                }
            }
        }
        HELIB_NTIMER_STOP(time_dpu_time_new_ifft_layer_data_from_dpu); // DIFF: use ifft timer
    }
    /* ---------------------------------------------------------------------------------------------------------- */


    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex array to std::complex array
    convert_from_dpu_complex(dpu_xp_start, blocks * size * 2, xp);
}

void dpu_new_fft_layer_2(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();
    dpu_new_fft_ifft_layer_2(xp, blocks, size, wtab, FFT);
}

void dpu_new_ifft_layer_2(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();
    dpu_new_fft_ifft_layer_2(xp, blocks, size, wtab, IFFT);
}

void dpu_fwd_butterfly_loop(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();

    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_size, last_dpu_size;

    GET_NR_DPUS_AND_SIZE_PER_DPU(size, nr_dpus, per_dpu_size, last_dpu_size);

    // Info("Using " + std::to_string(nr_dpus) + " dpus");
    // Info("Size " + std::to_string(size));
    // Info("Size per dpu " + std::to_string(per_dpu_size));
    // Info("Size last dpu " + std::to_string(last_dpu_size));

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_FWD_BUTTERFLY_LOOP, NULL));

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp0 = new dpu_complex[size];
    convert_to_dpu_complex(xp0, size, dpu_xp0);
    dpu_complex *dpu_xp1 = new dpu_complex[size];
    convert_to_dpu_complex(xp1, size, dpu_xp1);
    dpu_complex *dpu_wtab = new dpu_complex[size];
    convert_to_dpu_complex(wtab, size, dpu_wtab);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop_data_to_dpu);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
    dpu_transfer_size_to_dpu(set, nr_dpus, per_dpu_size, last_dpu_size, XSTR(DPU_SIZE));
    dpu_transfer_do_butterfly0_to_dpu(set, XSTR(DPU_DO_FWD_BUTTERFLY0));
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop);

    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop_data_from_dpu);

    // dpu_cycles_t dpu_cycles[nr_dpus];
    // DPU_FOREACH(set, dpu, each_dpu)
    // {
    //     DPU_ASSERT(dpu_prepare_xfer(dpu, &dpu_cycles[each_dpu]));
    // }
    // DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_CYCLES), 0, sizeof(dpu_cycles_t), DPU_XFER_DEFAULT));

    // for (uint32_t i = 0; i < nr_dpus; i++)
    // {
    //     std::cout << "dpu " << i << " - cycles: " << dpu_cycles[i].cycles[0] << std::endl;
    // }

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex arrays to std::complex arrays
    convert_from_dpu_complex(dpu_xp0, size, xp0);
    convert_from_dpu_complex(dpu_xp1, size, xp1);
}

void dpu_fwd_butterfly_loop_2(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();

    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_size, last_dpu_size;

    GET_NR_DPUS_AND_SIZE_PER_DPU(size, nr_dpus, per_dpu_size, last_dpu_size);

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_MULTI_FWD_BUTTERFLY_LOOP, NULL));

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp0 = new dpu_complex[size];
    convert_to_dpu_complex(xp0, size, dpu_xp0);
    dpu_complex *dpu_xp1 = new dpu_complex[size];
    convert_to_dpu_complex(xp1, size, dpu_xp1);
    dpu_complex *dpu_wtab = new dpu_complex[size];
    convert_to_dpu_complex(wtab, size, dpu_wtab);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop_data_to_dpu);
    uint32_t blocks = 1;
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
    dpu_transfer_size_to_dpu(set, nr_dpus, per_dpu_size, last_dpu_size, XSTR(DPU_SIZE));
    dpu_transfer_do_butterfly0_to_dpu(set, XSTR(DPU_DO_FWD_BUTTERFLY0));
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_BLOCKS), 0, &blocks, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop);

    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_fwd_butterfly_loop_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    HELIB_NTIMER_STOP(time_dpu_time_fwd_butterfly_loop_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex arrays to std::complex arrays
    convert_from_dpu_complex(dpu_xp0, size, xp0);
    convert_from_dpu_complex(dpu_xp1, size, xp1);
}

void dpu_inv_butterfly_loop(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();

    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_size, last_dpu_size;

    GET_NR_DPUS_AND_SIZE_PER_DPU(size, nr_dpus, per_dpu_size, last_dpu_size);

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_INV_BUTTERFLY_LOOP, NULL));

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp0 = new dpu_complex[size];
    convert_to_dpu_complex(xp0, size, dpu_xp0);
    dpu_complex *dpu_xp1 = new dpu_complex[size];
    convert_to_dpu_complex(xp1, size, dpu_xp1);
    dpu_complex *dpu_wtab = new dpu_complex[size];
    convert_to_dpu_complex(wtab, size, dpu_wtab);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop_data_to_dpu);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
    dpu_transfer_size_to_dpu(set, nr_dpus, per_dpu_size, last_dpu_size, XSTR(DPU_SIZE));
    dpu_transfer_do_butterfly0_to_dpu(set, XSTR(DPU_DO_INV_BUTTERFLY0));
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop);
    
    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex arrays to std::complex arrays
    convert_from_dpu_complex(dpu_xp0, size, xp0);
    convert_from_dpu_complex(dpu_xp1, size, xp1);
}

void dpu_inv_butterfly_loop_2(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab)
{
    LOG_FUNCTION();

    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_size, last_dpu_size;

    GET_NR_DPUS_AND_SIZE_PER_DPU(size, nr_dpus, per_dpu_size, last_dpu_size);

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY_MULTI_INV_BUTTERFLY_LOOP, NULL));

    // convert std::complex arrays to dpu_complex arrays
    dpu_complex *dpu_xp0 = new dpu_complex[size];
    convert_to_dpu_complex(xp0, size, dpu_xp0);
    dpu_complex *dpu_xp1 = new dpu_complex[size];
    convert_to_dpu_complex(xp1, size, dpu_xp1);
    dpu_complex *dpu_wtab = new dpu_complex[size];
    convert_to_dpu_complex(wtab, size, dpu_wtab);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop_data_to_dpu);
    uint32_t blocks = 1;
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_WTAB), dpu_wtab);
    dpu_transfer_size_to_dpu(set, nr_dpus, per_dpu_size, last_dpu_size, XSTR(DPU_SIZE));
    dpu_transfer_do_butterfly0_to_dpu(set, XSTR(DPU_DO_INV_BUTTERFLY0));
    DPU_ASSERT(dpu_broadcast_to(set, XSTR(DPU_BLOCKS), 0, &blocks, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop);

    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_inv_butterfly_loop_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP0), dpu_xp0);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_size, last_dpu_size, XSTR(DPU_XP1), dpu_xp1);
    HELIB_NTIMER_STOP(time_dpu_time_inv_butterfly_loop_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex arrays to std::complex arrays
    convert_from_dpu_complex(dpu_xp0, size, xp0);
    convert_from_dpu_complex(dpu_xp1, size, xp1);
}

void dpu_new_fft_last_two_layers(std::complex<double> *xp, long blocks, const char *dpu_binary)
{
    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_blocks, last_dpu_blocks;

    GET_NR_DPUS_AND_BLOCKS_PER_DPU(blocks, nr_dpus, per_dpu_blocks, last_dpu_blocks);

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, dpu_binary, NULL));

    // convert std::complex array to dpu_complex array
    dpu_complex *dpu_xp = new dpu_complex[blocks * 4];
    convert_to_dpu_complex(xp, blocks * 4, dpu_xp);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_new_fft_last_two_layers_data_to_dpu);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_blocks * 4, last_dpu_blocks * 4, XSTR(DPU_XP), dpu_xp);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (nr_dpus - 1 == each_dpu)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &last_dpu_blocks));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &per_dpu_blocks));
        }
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_BLOCKS), 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_new_fft_last_two_layers_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_new_fft_last_two_layers);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_new_fft_last_two_layers);

    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_new_fft_last_two_layers_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_blocks * 4, last_dpu_blocks * 4, XSTR(DPU_XP), dpu_xp);
    HELIB_NTIMER_STOP(time_dpu_time_new_fft_last_two_layers_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex array to std::complex array
    convert_from_dpu_complex(dpu_xp, blocks * 4, xp);
}

void dpu_new_fft_last_two_layers(std::complex<double> *xp, long blocks)
{
    LOG_FUNCTION();
    dpu_new_fft_last_two_layers(xp, blocks, DPU_BINARY_NEW_FFT_LAST_TWO_LAYERS);
}

void dpu_new_fft_last_two_layers_2(std::complex<double> *xp, long blocks)
{
    LOG_FUNCTION();
    dpu_new_fft_last_two_layers(xp, blocks, DPU_BINARY_MULTI_NEW_FFT_LAST_TWO_LAYERS);
}

void dpu_new_ifft_first_two_layers(std::complex<double> *xp, long blocks, const char *dpu_binary)
{
    struct dpu_set_t set, dpu;
    uint32_t each_dpu, nr_dpus, per_dpu_blocks, last_dpu_blocks;

    GET_NR_DPUS_AND_BLOCKS_PER_DPU(blocks, nr_dpus, per_dpu_blocks, last_dpu_blocks);

    DPU_ASSERT(dpu_alloc(nr_dpus, "backend=simulator", &set));
    DPU_ASSERT(dpu_load(set, dpu_binary, NULL));

    // convert std::complex array to dpu_complex array
    dpu_complex *dpu_xp = new dpu_complex[blocks * 4];
    convert_to_dpu_complex(xp, blocks * 4, dpu_xp);

    // transfer data to DPUs
    HELIB_NTIMER_START(time_dpu_time_new_ifft_first_two_layers_data_to_dpu);
    dpu_transfer_complex_array_to_dpu(set, per_dpu_blocks * 4, last_dpu_blocks * 4, XSTR(DPU_XP), dpu_xp);
    DPU_FOREACH(set, dpu, each_dpu)
    {
        if (nr_dpus - 1 == each_dpu)
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &last_dpu_blocks));
        }
        else
        {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &per_dpu_blocks));
        }
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, XSTR(DPU_BLOCKS), 0, sizeof(uint32_t), DPU_XFER_DEFAULT));
    HELIB_NTIMER_STOP(time_dpu_time_new_ifft_first_two_layers_data_to_dpu);

    // launch DPU program
    HELIB_NTIMER_START(time_dpu_time_new_ifft_first_two_layers);
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
    HELIB_NTIMER_STOP(time_dpu_time_new_ifft_first_two_layers);

    // copy data back from DPUs
    HELIB_NTIMER_START(time_dpu_time_new_ifft_first_two_layers_data_from_dpu);
    dpu_transfer_complex_array_from_dpu(set, per_dpu_blocks * 4, last_dpu_blocks * 4, XSTR(DPU_XP), dpu_xp);
    HELIB_NTIMER_STOP(time_dpu_time_new_ifft_first_two_layers_data_from_dpu);

    DPU_ASSERT(dpu_free(set));

    // convert dpu_complex array to std::complex array
    convert_from_dpu_complex(dpu_xp, blocks * 4, xp);
}

void dpu_new_ifft_first_two_layers(std::complex<double> *xp, long blocks)
{
    LOG_FUNCTION();
    dpu_new_ifft_first_two_layers(xp, blocks, DPU_BINARY_NEW_IFFT_FIRST_TWO_LAYERS);
}

void dpu_new_ifft_first_two_layers_2(std::complex<double> *xp, long blocks)
{
    LOG_FUNCTION();
    dpu_new_ifft_first_two_layers(xp, blocks, DPU_BINARY_MULTI_NEW_IFFT_FIRST_TWO_LAYERS);
}