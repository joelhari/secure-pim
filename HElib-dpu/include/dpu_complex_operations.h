
#ifndef _H_DPU_COMPLEX_OPERATIONS
#define _H_DPU_COMPLEX_OPERATIONS

#include "dpu_complex.h"

dpu_complex add(const dpu_complex a, const dpu_complex b)
{
  dpu_complex c = { a.real + b.real, a.imag + b.imag };
  return c;
}

dpu_complex sub(const dpu_complex a, const dpu_complex b)
{
  dpu_complex c = { a.real - b.real, a.imag - b.imag };
  return c;
}

dpu_complex mul(const dpu_complex a, const dpu_complex b)
{
  dpu_complex c = {
    a.real * b.real - a.imag * b.imag,
    a.real * b.imag + a.imag * b.real
  };
  return c;
}

dpu_complex cmul(const dpu_complex a, const dpu_complex b)
{
  dpu_complex c = {
    a.real * b.real + a.imag * b.imag,
    a.imag * b.real - a.real * b.imag
  };
  return c;
}

#endif
