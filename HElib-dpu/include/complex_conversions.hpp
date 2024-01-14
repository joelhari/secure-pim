
#ifndef _H_COMPLEX_CONVERSIONS
#define _H_COMPLEX_CONVERSIONS

#include <complex>
#include "dpu_complex.h"

dpu_complex convert_to_dpu_complex(const std::complex<double> x)
{
  dpu_complex dpu_x = { x.real(), x.imag() };
  return dpu_x;
}

void convert_to_dpu_complex(const std::complex<double> * x, long size, dpu_complex* dpu_x)
{
  for (long i = 0; i < size; i++)
  {
    dpu_x[i] = { x[i].real(), x[i].imag() };
  }
}

std::complex<double> convert_from_dpu_complex(const dpu_complex dpu_x)
{
  std::complex<double> x = std::complex<double>(dpu_x.real, dpu_x.imag);
  return x;
}

void convert_from_dpu_complex(const dpu_complex* x, long size, std::complex<double> * dpu_x)
{
  for (long i = 0; i < size; i++)
  {
    dpu_x[i] = std::complex<double>(x[i].real, x[i].imag);
  }
}

#endif
