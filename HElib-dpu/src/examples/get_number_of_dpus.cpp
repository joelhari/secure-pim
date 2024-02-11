
#include <dpu>
#include <iostream>

#define GET_MAX_NR_DPUS(max_nr_dpus)                         \
    {                                                        \
        struct dpu_set_t set;                                \
        DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &set)); \
        DPU_ASSERT(dpu_get_nr_dpus(set, &max_nr_dpus));      \
        DPU_ASSERT(dpu_free(set));                           \
    }


int main(int argc, char *argv[])
{
    uint32_t nr_dpus;

    GET_MAX_NR_DPUS(nr_dpus);

    std::cout << "Number of DPUs: " << nr_dpus << std::endl;

    return 0;
}
