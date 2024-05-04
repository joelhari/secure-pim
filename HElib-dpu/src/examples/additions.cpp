
#include <helib/helib.h>
#include <vector>
#include "print_timers.hpp"
#include "context_getter.h"

using namespace helib;

int main(int argc, char *argv[])
{
  uint32_t m, bits, c;
  GET_CKKS_CONTEXT(CKKS_CONTEXT_IDX)
  
#ifdef PRINT_SIZE
  std::cout << "Module,Method,DataType,Size" << std::endl;
#endif

  HELIB_NTIMER_START(time_total);

  Context context =
      ContextBuilder<CKKS>()
          .m(m)
          .bits(bits)
          .precision(20)
          .c(c)
          .build();

  // cout << "securityLevel=" << context.securityLevel() << "\n";

  long n = context.getNSlots();
  // cout << "Number of Slots: " << n << "\n";

  SecKey secretKey(context);
  secretKey.GenSecKey();
  const PubKey &publicKey = secretKey;

  size_t size = 10;
  std::vector<PtxtArray> ptxts;
  std::vector<PtxtArray> pptxts;
  std::vector<Ctxt> ctxts;
  for (int i = 0; i < size; ++i)
  {
    PtxtArray p1(context);
    p1.random();
    ptxts.push_back(p1);
    Ctxt c1(publicKey);
    p1.encrypt(c1);
    ctxts.push_back(c1);
  }

  /* DoubleCRT::use_dpu = true;

  for (int i = 0; i < size-1; ++i)
  {
    Ctxt c2 = ctxts[i];
    // std::cout << "-------------- start addition       --------------" << std::endl;
    HELIB_NTIMER_START(time_addition);
    c2 += ctxts[i+1];
    HELIB_NTIMER_STOP(time_addition);
    // std::cout << "-------------- stop addition        --------------" << std::endl;
    PtxtArray pp2(context);
    pp2.decrypt(c2, secretKey);
    pptxts.push_back(pp2);
  }

  DoubleCRT::use_dpu = false; */

/* #ifndef PRINT_SIZE
  for (int i = 0; i < size-1; ++i)
  {
    PtxtArray p2 = ptxts[i];
    p2 += ptxts[i+1];
    double distance = Distance(p2, pptxts[i]);
    std::cout << "[" << i << "] distance: " << distance << "\n";
  }
#endif */

  HELIB_NTIMER_STOP(time_total);

#ifndef PRINT_SIZE
  std::cout << "\n\n";
  helib::printNamedTimer(std::cout, "time_addition");
  helib::printNamedTimer(std::cout, "time_total");
  std::cout << "\n";
  print_configuration();
  print_timers();
#endif

  return 0;
}
