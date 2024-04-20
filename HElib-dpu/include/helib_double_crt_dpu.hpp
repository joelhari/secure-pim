
#ifndef _H_HELIB_DOUBLE_CRT_DPU
#define _H_HELIB_DOUBLE_CRT_DPU

#include <helib/DoubleCRT.h>

void dpu_add_ctxt(
    helib::IndexMap<NTL::vec_long>* this_map,
    const helib::IndexMap<NTL::vec_long>* other_map,
    const helib::Context* context,
    helib::IndexSet s,
    long phim);

void dpu_mul_ctxt(
    helib::IndexMap<NTL::vec_long>* this_map,
    const helib::IndexMap<NTL::vec_long>* other_map,
    const helib::Context* context,
    helib::IndexSet s,
    long phim);

void dpu_toPoly(NTL::ZZX& poly, const helib::IndexSet& s, bool positive);

#endif // _H_HELIB_DOUBLE_CRT_DPU
