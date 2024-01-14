#include <stdio.h>
#include <mram.h>
#include "dpu_complex.h"

typedef dpu_complex complex;

__host complex a;
__host complex b;

void MUL(void)
{
    double x = a.real, y = a.imag, u = b.real, v = b.imag;
    a.real = x * u - y * v;
    a.imag = x * v + y * u;
}

int main(void)
{
    MUL();
    return 0;
}
