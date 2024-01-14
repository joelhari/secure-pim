
#include <dpu>
#include <iostream>
#include <helib/log.h>
#include <helib/timing.h>
#include <helib/helib.h>
#include <helib/Ctxt.h>
#include "helib_ctxt_dpu.hpp"

#define DPU_BINARY_DIR "./build/dpu/"
#define DPU_BINARY_ADD_CTXT DPU_BINARY_DIR "dpu_add_ctxt"

using namespace helib;

long getPartIndexByHandle(std::vector<CtxtPart> parts, const SKHandle &handle)
{
    for (size_t i = 0; i < parts.size(); i++)
        if (parts[i].skHandle == handle)
            return i;
    return -1;
}

void dpu_addCtxt(std::vector<CtxtPart> parts, std::vector<CtxtPart> otherParts)
{
    size_t size = otherParts.size();
    long partIndices[size];
    for (size_t i = 0; i < size; i++) {
        partIndices[i] = getPartIndexByHandle(parts, otherParts[i].skHandle);
    }

    std::cout << "size: " << size << std::endl;


}
