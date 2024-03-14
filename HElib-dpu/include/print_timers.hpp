
#ifndef _H_PRINT_TIMERS
#define _H_PRINT_TIMERS

#include <iostream>
#include <helib/timing.h>

void print_configuration()
{
#ifdef PGFFT_DISABLE_SIMD
    std::cout << "PGFFT_DISABLE_SIMD: " << PGFFT_DISABLE_SIMD << std::endl;
#else
    std::cout << "PGFFT_DISABLE_SIMD: undefined" << std::endl;
#endif
#ifdef USE_DPU
    std::cout << "USE_DPU: " << USE_DPU << std::endl;
#else
    std::cout << "USE_DPU: undefined" << std::endl;
#endif
#ifdef USE_MULTI_TASKLETS
    std::cout << "USE_MULTI_TASKLETS: " << USE_MULTI_TASKLETS << std::endl;
#else
    std::cout << "USE_MULTI_TASKLETS: undefined" << std::endl;
#endif
}

void print_timers()
{
    std::cout << "\nPGFFT Method Timers:\n";
    helib::printNamedTimer(std::cout, "time_fwd_butterfly_loop");
    helib::printNamedTimer(std::cout, "time_dpu_time_fwd_butterfly_loop");
    helib::printNamedTimer(std::cout, "time_dpu_time_fwd_butterfly_loop_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_fwd_butterfly_loop_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_new_fft_layer");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_layer");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_layer_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_layer_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_new_fft_last_two_layers");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_last_two_layers");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_last_two_layers_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_fft_last_two_layers_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_inv_butterfly_loop");
    helib::printNamedTimer(std::cout, "time_dpu_time_inv_butterfly_loop");
    helib::printNamedTimer(std::cout, "time_dpu_time_inv_butterfly_loop_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_inv_butterfly_loop_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_new_ifft_layer");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_layer");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_layer_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_layer_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_new_ifft_first_two_layers");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_first_two_layers");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_first_two_layers_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_new_ifft_first_two_layers_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_cobra");
    helib::printNamedTimer(std::cout, "time_dpu_time_cobra");
    helib::printNamedTimer(std::cout, "time_dpu_time_cobra_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_cobra_data_from_dpu");
    std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_basic_bit_reverse_copy");
    helib::printNamedTimer(std::cout, "time_dpu_time_basic_bit_reverse_copy");
    helib::printNamedTimer(std::cout, "time_dpu_time_basic_bit_reverse_copy_data_to_dpu");
    helib::printNamedTimer(std::cout, "time_dpu_time_basic_bit_reverse_copy_data_from_dpu");
    std::cout << std::endl;
    // helib::printNamedTimer(std::cout, "time_addCtxt");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_1");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_2");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_3");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_4");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_5");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_6");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_7");
    // helib::printNamedTimer(std::cout, "time_addCtxt_part_8");
    // std::cout << std::endl;
    helib::printNamedTimer(std::cout, "time_dpu_time_hello_world");
    helib::printNamedTimer(std::cout, "time_dpu_time_mul");
}

#endif
