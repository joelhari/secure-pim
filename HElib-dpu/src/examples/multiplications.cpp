
#include <helib/helib.h>
#include <vector>
#include "print_timers.hpp"

using namespace helib;

int main(int argc, char *argv[])
{
  HELIB_NTIMER_START(time_total);

  Context context =
      ContextBuilder<CKKS>()
          .m(16 * 1024)
          .bits(119)
          .precision(20)
          .c(2)
          .build();

  // cout << "securityLevel=" << context.securityLevel() << "\n";

  long n = context.getNSlots();
  // cout << "Number of Slots: " << n << "\n";

  SecKey secretKey(context);
  secretKey.GenSecKey();
  const PubKey &publicKey = secretKey;

  size_t size = 1000;
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

  for (int i = 0; i < size-1; ++i)
  {
    Ctxt c2 = ctxts[i];
    HELIB_NTIMER_START(time_multiplication);
    c2 *= ctxts[i+1];
    HELIB_NTIMER_STOP(time_multiplication);
    PtxtArray pp2(context);
    pp2.decrypt(c2, secretKey);
    pptxts.push_back(pp2);
  }

  for (int i = 0; i < size-1; ++i)
  {
    PtxtArray p2 = ptxts[i];
    p2 *= ptxts[i+1];
    double distance = Distance(p2, pptxts[i]);
    std::cout << "[" << i << "] distance: " << distance << "\n";
  }

  HELIB_NTIMER_STOP(time_total);

  std::cout << "\n\n";
  helib::printNamedTimer(std::cout, "time_multiplication");
  helib::printNamedTimer(std::cout, "time_total");
  std::cout << "\n";
  print_configuration();
  print_timers();

  return 0;
}
