
#ifndef _H_HELIB_PGFFT_DPU
#define _H_HELIB_PGFFT_DPU

#include <complex>

void dpu_new_fft_layer(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab);

void dpu_new_fft_layer_2(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab);

void dpu_new_ifft_layer(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab);

void dpu_new_ifft_layer_2(
    std::complex<double> *xp, long blocks, long size,
    const std::complex<double> *wtab);

void dpu_fwd_butterfly_loop(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab);

void dpu_fwd_butterfly_loop_2(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab);

void dpu_inv_butterfly_loop(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab);

void dpu_inv_butterfly_loop_2(
    long size,
    std::complex<double> *xp0,
    std::complex<double> *xp1,
    const std::complex<double> *wtab);

void dpu_new_fft_last_two_layers(std::complex<double> *xp, long blocks);

void dpu_new_fft_last_two_layers_2(std::complex<double> *xp, long blocks);

void dpu_new_ifft_first_two_layers(std::complex<double> *xp, long blocks);

void dpu_new_ifft_first_two_layers_2(std::complex<double> *xp, long blocks);

#endif // _H_HELIB_PGFFT_DPU
