
#include <helib/helib.h>

#include "print_timers.hpp"

using namespace std;
using namespace helib;

int main(int argc, char *argv[])
{
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

  vector<double> v0(n);
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
  Info("-------------- start addition       --------------");
  c2 += c1;
  Info("-------------- stop addition        --------------");

  Ctxt c3 = c0;
  Info("-------------- start multiplication --------------");
  HELIB_NTIMER_START(time_mul);
  c3 *= c1;
  HELIB_NTIMER_STOP(time_mul);
  helib::printNamedTimer(std::cout, "time_mul");
  Info("-------------- stop multiplication  --------------");

  PtxtArray pp2(context);
  pp2.decrypt(c2, secretKey);
  // vector<double> v2;
  // pp2.store(v2);

  PtxtArray p2 = p0;
  p2 += p1;

  PtxtArray pp3(context);
  pp3.decrypt(c3, secretKey);
  // vector<double> v3;
  // pp3.store(v3);

  PtxtArray p3 = p0;
  p3 *= p1;

  double distance_add = Distance(p2, pp2);
  cout << "distance addition = " << distance_add << "\n";

  double distance_mul = Distance(p3, pp3);
  cout << "distance multiplication = " << distance_mul << "\n";

  print_timers();

  return 0;
}
