#include <helib/helib.h>
#include "helib/DoubleCRT.h"
#include "test_utils.h"
#include "helib_double_crt_dpu.hpp"
#include "context_getter.h"

#define TEST_RUNS 50
#define RANDOM_SEED 17

using namespace helib;

std::vector<long> get_result(CtxtPart &part)
{
    std::vector<long> res;
    auto map1 = part.getMap();
    const IndexSet &s = map1.getIndexSet();
    for (long i = s.first(); i <= s.last(); i = s.next(i))
    {
        for (long j = 0; j < map1[i].length(); j++)
        {
            res.push_back(map1[i].at(j));
        }
    }
    return res;
}

void check_equals(std::vector<long> xs, std::vector<long> ys)
{
    for (long i = 0; i < xs.size() || i < ys.size(); i++)
    {
        if (xs.at(i) != ys.at(i))
        {
            std::cout << "    ERROR: " << __FUNCTION__ << " failed on line " << __LINE__ << " at index [" << i << "]:  " << ys.at(i) << " instead of " << xs.at(i) << std::endl;
        }
    }
}

void test_dpu_add_ctxt()
{
    uint32_t m, bits, c;
    GET_CKKS_CONTEXT(0);

    Context context =
        ContextBuilder<CKKS>()
            .m(m)
            .bits(bits)
            .precision(20)
            .c(c)
            .build();

    SecKey secretKey(context);
    secretKey.GenSecKey();
    const PubKey &publicKey = secretKey;

    long n = context.getNSlots();
    std::vector<double> v0(n);
    for (long i = 0; i < n; i++)
        v0[i] = sin(2.0 * PI * i / n);

    PtxtArray p0(context, v0);
    Ctxt c0(publicKey);
    p0.encrypt(c0);

    PtxtArray p1(context);
    p1.random();
    Ctxt c1(publicKey);
    p1.encrypt(c1);

    Ctxt c2 = c0;
    Ctxt c2_dpu = c0;

    std::cout << "------------ helib ------------" << std::endl;
    CtxtPart &part = c2.test_add_part_0(c1);
    std::vector<long> res = get_result(part);
    std::cout << "------------ dpu ------------" << std::endl;
    CtxtPart &part_dpu = c2_dpu.test_add_part_0_dpu(c1);
    std::vector<long> res_dpu = get_result(part_dpu);
    std::cout << "------------ end ------------" << std::endl;

    check_equals(res, res_dpu);
}

int main(void)
{
#ifdef RANDOM_SEED
    std::srand(RANDOM_SEED);
#endif

    for (size_t i = 0; i < TEST_RUNS; i++)
    {
        std::cout << "running test iteration " << i + 1 << "/" << TEST_RUNS << "\r";
        std::cout.flush();
        test_dpu_add_ctxt();
    }

    return 0;
}
