
#ifndef _H_TEST_UTILS
#define _H_TEST_UTILS

#include <complex>
#include <cstdlib>
#include <iostream>

#define ITERATIONS 10000

#define LOG_FAILURE(is, expected) \
    std::cout << "    ERROR: " << __FUNCTION__ << " failed on line " << __LINE__ << ":  " << is << " instead of " << expected << std::endl;

#define IS_TRUE(x)               \
    {                            \
        if (!(x))                \
            LOG_FAILURE(x, true) \
    }

#define EQUALS(a, b)                                     \
    {                                                    \
        if ((a - b) < -0.0000001 || (a - b) > 0.0000001) \
            LOG_FAILURE(a, b)                            \
    }

#define EQUALS_COMPLEX(x, dpu_x)                                                                                                                                                                 \
    {                                                                                                                                                                                            \
        if ((x.real() - dpu_x.real) < -0.00000001 || (x.real() - dpu_x.real) > 0.0000001 || (x.imag() - dpu_x.imag) < -0.0000001 || (x.imag() - dpu_x.imag) > 0.0000001)                         \
            std::cout << "    ERROR: " << __FUNCTION__ << " failed on line " << __LINE__ << ":  " << x.real() << " + " << x.imag() << "i != " << dpu_x.real << " + " << dpu_x.imag << "i" << std::endl; \
    }

#define EQUALS_COMPLEX_ARRAY(xs, dpu_xs, size)                                                                                                                                                                       \
    {                                                                                                                                                                                                                \
        for (size_t i = 0; i < size; i++)                                                                                                                                                                            \
        {                                                                                                                                                                                                            \
            if ((xs[i].real() - dpu_xs[i].real) < -0.00000001 || (xs[i].real() - dpu_xs[i].real) > 0.0000001 || (xs[i].imag() - dpu_xs[i].imag) < -0.0000001 || (xs[i].imag() - dpu_xs[i].imag) > 0.0000001)         \
            {                                                                                                                                                                                                        \
                std::cout << "    ERROR: " << __FUNCTION__ << " failed on line " << __LINE__ << ":  " << xs[i].real() << " + " << xs[i].imag() << "i != " << dpu_xs[i].real << " + " << dpu_xs[i].imag << "i" << std::endl; \
            }                                                                                                                                                                                                        \
        }                                                                                                                                                                                                            \
    }

#define EQUALS_STD_COMPLEX_ARRAY(xs, ys, size)                                                                                                                                                                                               \
    {                                                                                                                                                                                                                                        \
        for (size_t i = 0; i < size; i++)                                                                                                                                                                                                    \
        {                                                                                                                                                                                                                                    \
            if ((xs[i].real() - ys[i].real()) < -0.00000001 || (xs[i].real() - ys[i].real()) > 0.0000001 || (xs[i].imag() - ys[i].imag()) < -0.0000001 || (xs[i].imag() - ys[i].imag()) > 0.0000001)                                         \
            {                                                                                                                                                                                                                                \
                std::cout << "    ERROR: " << __FUNCTION__ << " failed on line " << __LINE__ << ": at array index " << i << ":  " << xs[i].real() << " + " << xs[i].imag() << "i != " << ys[i].real() << " + " << ys[i].imag() << "i" << std::endl; \
            }                                                                                                                                                                                                                                \
        }                                                                                                                                                                                                                                    \
    }

void copy_complex_array(std::complex<double> *source, std::complex<double> *target, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        target[i] = std::complex<double>(source[i]);
    }
}

double rand_double(double d_min = 0.0, double d_max = 1.0)
{
    double d = (double)std::rand() / RAND_MAX;
    return d_min + d * (d_max - d_min);
}

#endif
