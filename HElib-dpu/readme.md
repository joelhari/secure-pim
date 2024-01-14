# HElib DPU

This Folder contains [HElib example programs](./src/examples), [DPU host code](./src/helib_pgfft_dpu.cpp), [DPU programs](./src/dpu) and [Tests](./src/tests).

The example programs will be compiled with the HElib library. The HElib library is in the [HElib directory](../HElib), which has been refactored to run certain methods on the DPUs. These methods are implemented in the [DPU programs](./src/dpu).

## HElib



## DPU Programs

The [DPU programs](./src/dpu) folder contains programs implementing one of the HElib methods. They are named after the method implemented, e.g. `dpu_<method_name>.c`.  
The programs whose name start with `dpu_multi_...` can be compiled and run to use multiple tasklets.  
The programs in the [single-tasklet](./src/dpu/single-tasklet/) subfolder can only run using one tasklet. We've implemented these first, before considering the use of multiple tasklets. These are legacy programs which we keep in order to compare the performance to the newer implementations.

## Compile

### Example Programs

To compile the [HElib example programs](./src/examples) run (inside this directory):
``` terminal
make all -j8 NR_DPUS=64 USE_DPU=1 USE_MULTI_TASKLETS=1
```
The arguments are:
- `NR_DPUS=[1, MAX_AVAILABLE_DPUs]`: number of DPUs that will be used
- `USE_DPU=[0, 1]`: if the original HElib implementation `=0` or the DPU implementation `=1` will be used
- `USE_MULTI_TASKLETS=[0, 1]`: if the DPU programs that can only run with one tasklet `=0` or those that can run with multiple tasklets `=1` will be used (if available)

When changing any of the above arguments, make sure to run `make clean` before in order for the changed arguments to be applyed correctly.

### DPU Programs

To compile the [DPU programs](./src/dpu) run (inside this directory):
``` terminal
make dpu -j8 NR_TASKLETS=8
```
The arguments are:
- `NR_TASKLETS=[1, 2, 4, 8]`: number of tasklets to use

The number of tasklets must be a 1 or a multiple of 2. It cannot be more than 8 because of the space in the WRAM each tasklet uses with the current implementation.

## Run

To run any of the example programs execute:
``` terminal
./build/<name_of_example_program>
```

## Tests

### Compile

To compile the [Tests](./src/tests) run (inside this directory):
``` terminal
make tests -j8 NR_DPUS=64 USE_DPU=0 USE_MULTI_TASKLETS=1
```
The arguments are:
- `NR_DPUS=[1, MAX_AVAILABLE_DPUs]`: number of DPUs that will be used
- `USE_DPU=0`: make sure to compile with `USE_DPU=0` for the tests to run using the original HElib implementation
- `USE_MULTI_TASKLETS=[0, 1]`: if the DPU programs that can only run with one tasklet `=0` or those that can run with multiple tasklets `=1` will be used

When changing any of the above arguments, make sure to run `make clean` before in order for the changed arguments to be applyed correctly.

### Run

To execute the tests run
``` terminal
./build/<name_of_test>
```
or execute all tests at once using the [`run_all_tests.sh`](run_all_tests.sh) script:
``` terminal
./run_all_tests.sh
```
