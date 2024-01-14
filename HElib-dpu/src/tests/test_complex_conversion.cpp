#include <iostream>
#include <complex>

#include "test_utils.h"

#define TEST_RUNS 10

typedef std::complex<double> cmplx_t;

namespace test {

#include "complex_conversions.hpp"

void test_convert_to_dpu_complex(void)
{
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        cmplx_t x(rand_double(), rand_double());

        dpu_complex dpu_x = convert_to_dpu_complex(x);

        EQUALS_COMPLEX(x, dpu_x);
    }

    cmplx_t xs[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);

    EQUALS_COMPLEX_ARRAY(xs, dpu_xs, ITERATIONS);
}

void test_convert_from_dpu_complex(void)
{
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_complex dpu_x = {rand_double(), rand_double()};

        cmplx_t x = convert_from_dpu_complex(dpu_x);

        EQUALS_COMPLEX(x, dpu_x);
    }

    dpu_complex dpu_xs[ITERATIONS];
    cmplx_t xs[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = {rand_double(), rand_double()};
    }

    convert_from_dpu_complex(dpu_xs, ITERATIONS, xs);

    EQUALS_COMPLEX_ARRAY(xs, dpu_xs, ITERATIONS);
}

void test_convert_to_and_from_complex(void)
{
    cmplx_t xs[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_addition(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = add(dpu_xs[i], dpu_ys[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] += ys[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_multiple_additions(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = add(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = add(dpu_xs[i], dpu_xs[i]);
        dpu_xs[i] = add(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = add(dpu_xs[i], dpu_xs[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] += ys[i];
        xs[i] += xs[i];
        xs[i] += ys[i];
        xs[i] += xs[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_sub(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = sub(dpu_xs[i], dpu_ys[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] -= ys[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_multiple_sub(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = sub(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = sub(dpu_xs[i], dpu_xs[i]);
        dpu_xs[i] = sub(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = sub(dpu_xs[i], dpu_xs[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] -= ys[i];
        xs[i] -= xs[i];
        xs[i] -= ys[i];
        xs[i] -= xs[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_mul(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = mul(dpu_xs[i], dpu_ys[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] *= ys[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_multiple_mul(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = mul(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = mul(dpu_xs[i], dpu_xs[i]);
        dpu_xs[i] = mul(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = mul(dpu_xs[i], dpu_xs[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] *= ys[i];
        xs[i] *= xs[i];
        xs[i] *= ys[i];
        xs[i] *= xs[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

void test_convert_to_and_from_complex_with_calc(void)
{
    cmplx_t xs[ITERATIONS];
    cmplx_t ys[ITERATIONS];
    dpu_complex dpu_xs[ITERATIONS];
    dpu_complex dpu_ys[ITERATIONS];
    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] = cmplx_t(rand_double(), rand_double());
        ys[i] = cmplx_t(rand_double(), rand_double());
    }

    convert_to_dpu_complex(xs, ITERATIONS, dpu_xs);
    convert_to_dpu_complex(ys, ITERATIONS, dpu_ys);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        dpu_xs[i] = mul(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = sub(dpu_xs[i], dpu_xs[i]);
        dpu_xs[i] = add(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = mul(dpu_xs[i], dpu_xs[i]);
        dpu_xs[i] = add(dpu_xs[i], dpu_ys[i]);
        dpu_xs[i] = sub(dpu_xs[i], dpu_xs[i]);
    }

    cmplx_t res[ITERATIONS];
    convert_from_dpu_complex(dpu_xs, ITERATIONS, res);

    for (size_t i = 0; i < ITERATIONS; i++)
    {
        xs[i] *= ys[i];
        xs[i] -= xs[i];
        xs[i] += ys[i];
        xs[i] *= xs[i];
        xs[i] += ys[i];
        xs[i] -= xs[i];
    }

    EQUALS_STD_COMPLEX_ARRAY(xs, res, ITERATIONS)
}

}


int main(void)
{
    for (size_t i = 0; i < TEST_RUNS; i++)
    {
        std::cout << "running test iteration " << i + 1 << "/" << TEST_RUNS << "\r";
        test::test_convert_to_dpu_complex();
        test::test_convert_from_dpu_complex();
        test::test_convert_to_and_from_complex();
        test::test_convert_to_and_from_complex_with_addition();
        test::test_convert_to_and_from_complex_with_multiple_additions();
        test::test_convert_to_and_from_complex_with_sub();
        test::test_convert_to_and_from_complex_with_multiple_sub();
        test::test_convert_to_and_from_complex_with_mul();
        test::test_convert_to_and_from_complex_with_multiple_mul();
        test::test_convert_to_and_from_complex_with_calc();
        std::cout.flush();
    }

    return 0;
}
