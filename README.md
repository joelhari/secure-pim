# secure-pim

## Setup

### Prerequisites
- Ubuntu 22.04 LTS
- GNU make >= 4.3
- g++ >= 11.3.0
- m4 >= 1.4.18
- pkg-config >= 0.29.0

```
sudo apt install make g++ m4 pkg-config
```

### GMP
- Download, decompress, build, and install `gmp`. It will be used as a shared library by HElib.
```
wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz 
tar -xvf gmp-6.3.0.tar.xz
cd gmp-6.3.0
./configure --prefix=$HOME/libs
make 
sudo make install
sudo ldconfig
```
- This will build and install `gmp` in `/usr/local/lib` by default.

### NTL
- Download, decompress, build, and install `ntl`. It will also be used as a shared library by HElib.
```
wget https://libntl.org/ntl-11.5.1.tar.gz
tar -xvf ntl-11.5.1.tar.gz
cd ntl-11.5.1/src
./configure NTL_GMP_LIP=on SHARED=on  NTL_THREADS=on NTL_THREAD_BOOST=on PREFIX=$HOME/libs GMP_PREFIX=$HOME/libs
make
sudo make install
```
- This will build and install `ntl` in `/usr/local/lib` by default.

### UPMEM DPU toolchain
- Download, decompress, and install the UPMEM DPU toolchain.
```
wget http://sdk-releases.upmem.com/2023.2.0/ubuntu_22.04/upmem-2023.2.0-Linux-x86_64.tar.gz
mkdir $HOME/upmem-sdk
tar -xvf upmem-2023.2.0-Linux-x86_64.tar.gz -C $HOME/upmem-sdk
```
- Source the environment.
```
source $HOME/upmem-sdk/upmem-2023.2.0-Linux-x86_64/upmem_env.sh
```

### Library paths
- Make sure the library paths are set.
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
```

## Running the Tests

After the [Setup](#setup), run the `run-all.sh <nr-of-dpus>` script, passing the number
of available DPUs in the system as the argument. The script will run the `execute-tests.sh`
and `get-sys-info.sh` scripts and save the output to `execute-tests.log` and `get-sys-info.log`.
```
./run-all.sh <nr-of-dpus>
```


Alternatively you can run those scripts individually (does not create .log file):

#### Execute Tests
Run the `execute-tests.sh` script to execute the tests.
It takes care of compiling with the right configurations, executing the test programs,
and saving the results.
```
./execute-tests.sh <nr-of-dpus>
```
The results of the tests will be stored in the [results](/results) folder.  
The file names encode which test program was run with which configuration:  
`res-<test-program-name>-<use-dpu>-<disable-simd/nr-tasklets>-<nr-dpus>.txt`

#### Gather system info
Run the `get-sys-info.sh` script to gather relevant system information (i.e. CPU, Memory).
It will save the system info in the [sysinfo](/sysinfo) folder.
```
./get-sys-info.sh
```

## Compile

A detailed description on how to compile and run the programs and unit tests,
see the [README](/HElib-dpu/readme.md) file inside the [HElib-dpu](/HElib-dpu/) directory.
