#include <stdio.h>
#include <mram.h>
#include <perfcounter.h>
#include <defs.h>
#include "common.h"

// __mram_noinit ... DPU_PARTS[BUFFER_SIZE];
// __mram_noinit ... DPU_OTHER_PARTS[BUFFER_SIZE];
__host uint32_t DPU_SIZE;
__host uint8_t DPU_NEGATIVE;

#define SP_CORRECT_EXCESS(a, n)        \
    {                                  \
        return a - n >= 0 ? a - n : a; \
    }

void ctxt_add(/* part, otherPart */)
{
}

int main(void)
{
    uint8_t idx = me();

    // min. size per tasklet is 4 => we can use a maximum of "(size / 4) * blocks" DPUs
    uint8_t nr_tasklets = NR_TASKLETS;
    while (nr_tasklets > DPU_SIZE)
    {
        nr_tasklets /= 2;
    }

    uint32_t per_tasklet_size = DPU_SIZE / nr_tasklets;

    if (idx >= nr_tasklets)
    {
        return 0;
    }


    return 0;
}
