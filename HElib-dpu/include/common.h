#ifndef _H_COMMON
#define _H_COMMON

#include <stdint.h>
#include <dpu_complex.h>

#define XSTR(x) STR(x)
#define STR(x) #x

/* DPU variable that will be read of write by the host */
#define DPU_CYCLES dpu_wram_cycles

#define DPU_XP                 mram_xp
#define DPU_XP0                mram_xp0
#define DPU_XP1                mram_xp1
#define DPU_WTAB               mram_wtab
#define DPU_SIZE               size
#define DPU_BLOCKS             blocks
#define DPU_DO_FWD_BUTTERFLY0  do_fwd_butterfly0
#define DPU_DO_INV_BUTTERFLY0  do_inv_butterfly0

#define DPU_PARTS              parts
#define DPU_OTHER_PARTS        other_parts
#define DPU_NEGATIVE           negative

/* Make large numbers easier to read (and accurate) */
/* copied from https://github.com/UBC-ECE-Sasha/PIM-common/tree/master/common/include/common.h */
#define KILOBYTE(_x) (_x << 10)
#define MEGABYTE(_x) (_x << 20)
#define GIGABYTE(_x) (_x << 30)
#define TERABYTE(_x) (_x << 40)

#define WRAM_SIZE KILOBYTE(64)
#define MRAM_SIZE MEGABYTE(64)

/* Size of the buffer on which the checksum will be performed */
#define BLOCK_SIZE        4
#define BUFFER_SIZE       MRAM_SIZE / 8 / sizeof(dpu_complex)
#define LOCAL_CACHE_SIZE  BLOCK_SIZE * sizeof(dpu_complex)




/* Structure used by both the host and the dpu to communicate information */

/* typedef struct {
    uint32_t checksum;
    uint32_t cycles;
} dpu_cycles_t; */

typedef struct {
    uint32_t cycles[NR_TASKLETS];
} dpu_cycles_t;

#endif /* _H_COMMON */
