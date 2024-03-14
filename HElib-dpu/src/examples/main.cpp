
#include <helib/helib.h>

#include "print_timers.hpp"

using namespace std;
using namespace helib;

int main(int argc, char *argv[])
{
  HELIB_NTIMER_START(time_total);

  Context context =
      ContextBuilder<CKKS>()
          .m(16 * 1024)
          // m is the "cyclotomic index". For CKKS, m must be a power of 2.  As
          // m increases, you get more security and more slots, but the
          // performance degrades and the size of a ciphertext increases. See
          // table below for more information.

          .bits(119)
          // bits specifies the number of bits in the "ciphertext modulus".  As
          // bits increases, you get less security, but you can perform deeper
          // homomorphic computations; in addition, the size of a ciphertext
          // increases.  See table below for more information. Also see
          // 02_depth.cpp for more information about how depth and bits are
          // related.

          .precision(20)
          // precision specifies the number of bits of precision when data is
          // encoded, encrypted, or decrypted.  More precisely, each of these
          // operations are designed to add an error term of at most
          // 2^{-precision} to each slot.  As precision increases, the allowed
          // depth of homomorphic computations decreases (but security and
          // performance are not affected).  It is not recommended to use
          // precision greater than about 40 or so.

          .c(2)
          // c specifies the number of columns in key-switching matrices.  Yes,
          // it sounds very technical, and it is.  However, all you have to know
          // about this parameter is that as c increases, you get a little more
          // security, but performance degrades and the memory requirement for
          // the public key increases. c must be at least 2 and it is not
          // recommended to set c higher than 8.  See table below for more
          // information.
          
          .build();

  // The following table lists settings of m, bits, and c that yield (at least)
  // 128-bit security.  It is highly recommended to only use settings from this
  // table.
  //
  // m       bits  c
  //  16384   119  2
  //  32768   358  6
  //  32768   299  3
  //  32768   239  2
  //  65536   725  8
  //  65536   717  6
  //  65536   669  4
  //  65536   613  3
  //  65536   558  2
  // 131072  1445  8
  // 131072  1435  6
  // 131072  1387  5
  // 131072  1329  4
  // 131072  1255  3
  // 131072  1098  2
  // 262144  2940  8
  // 262144  2870  6
  // 262144  2763  5
  // 262144  2646  4
  // 262144  2511  3
  // 262144  2234  2

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
  HELIB_NTIMER_START(time_addition);
  c2 += c1;
  HELIB_NTIMER_STOP(time_addition);
  Info("-------------- stop addition        --------------");

  Ctxt c3 = c0;
  Info("-------------- start multiplication --------------");
  HELIB_NTIMER_START(time_multiplication);
  c3 *= c1;
  HELIB_NTIMER_STOP(time_multiplication);
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

  HELIB_NTIMER_STOP(time_total);

  std::cout << "\n\n";
  helib::printNamedTimer(std::cout, "time_addition");
  helib::printNamedTimer(std::cout, "time_multiplication");
  helib::printNamedTimer(std::cout, "time_total");
  std::cout << "\n";
  print_configuration();
  print_timers();

  return 0;
}
