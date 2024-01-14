#
#
# Modified Makefile to build HElib refactored for DPUs.
# Peterson Yuhala <petersonyuhala@gmail.com>
# Joel Hari <joel@familiehari.ch>
#

NR_TASKLETS ?= 16
NR_DPUS ?= 1
USE_DPU ?= 0
USE_MULTI_TASKLETS ?= 0
PGFFT_DISABLE_SIMD ?= 1

USE_DPU_FLAG := 
ifeq ($(USE_DPU), 1)
	USE_DPU_FLAG := -DUSE_DPU
endif

USE_MULTI_TASKLETS_FLAG := 
ifeq ($(USE_MULTI_TASKLETS), 1)
	USE_MULTI_TASKLETS_FLAG := -DUSE_MULTI_TASKLETS
endif

PGFFT_DISABLE_SIMD_FLAG := 
ifeq ($(PGFFT_DISABLE_SIMD), 1)
	PGFFT_DISABLE_SIMD_FLAG := -DPGFFT_DISABLE_SIMD
endif

CC = g++
LD = g++
AR = ar
ARFLAGS = rv
GMP = -lgmp
NTL = -lntl
COPT = -g -O2 -march=native
INC_HELIB = -I../HElib/include/
INCLUE = -Iinclude

LIBS_DPU := `dpu-pkg-config --cflags --libs dpu` -g -DNR_TASKLETS=$(NR_TASKLETS) -DNR_DPUS=$(NR_DPUS) $(USE_DPU_FLAG) $(USE_MULTI_TASKLETS_FLAG) $(PGFFT_DISABLE_SIMD_FLAG)

# INC and LIB changed to when GMP and/or NTL not in default locations
# Put these definitions in the file local-defs, which will be included
# if found.  ====> PUT ONLY *ABSOLUTE PATHS* IN local-defs <====
# You can also overwrite COPT in this local file.
# See local-defs.example for an example.
# include $(wildcard local-defs)

CFLAGS = -pthread $(COPT) $(INC_HELIB) $(INCLUE) -DHELIB_THREADS -DHELIB_BOOT_THREADS -fmax-errors=1 -Werror=return-type -std=c++17

LDFLAGS =  -lgmp -lntl -lm -lpthread

# COMMON_FLAGS := -Wall -Wextra -Werror -g -I${COMMON_INCLUDES}
# HOST_FLAGS := ${COMMON_FLAGS} -std=c11 -O3 `dpu-pkg-config --cflags --libs dpu` -DNR_TASKLETS=${NR_TASKLETS} -DNR_DPUS=${NR_DPUS}
# DPU_FLAGS := ${COMMON_FLAGS} -O2 -DNR_TASKLETS=${NR_TASKLETS}


# useful flags:
#   -DHELIB_THREADS  tells helib to enable generic multithreading capabilities;
#                    must be used with a thread-enabled NTL and the -pthread
#                    flag should be passed to gcc
#
#   -DHELIB_BOOT_THREADS  tells helib to use a multithreading strategy for
#                         bootstrapping; requires -DHELIB_THREADS (see above)

# $(info HElib requires NTL version 11.4.3 or higher, see http://shoup.net/ntl)
# $(info )

SRC_DIR := ./src
SRCS    := $(wildcard $(SRC_DIR)/*.cpp)
OBJS    := $(SRCS:%.cpp=%.o)

HELIB_SRC_DIR := ../HElib/src
HELIB_SRCS    := $(wildcard $(HELIB_SRC_DIR)/*.cpp)
HELIB_OBJS    := $(HELIB_SRCS:%.cpp=%.o)

EXAMPLES_SRC_DIR := ./src/examples
EXAMPLES_SRCS    := $(wildcard $(EXAMPLES_SRC_DIR)/*.cpp)
EXAMPLES_OBJS    := $(EXAMPLES_SRCS:%.cpp=%.o)
EXAMPLES_TARGETS := $(EXAMPLES_SRCS:$(EXAMPLES_SRC_DIR)/%.cpp=%)

TESTS_SRC_DIR := ./src/tests
TESTS_SRCS    := $(wildcard $(TESTS_SRC_DIR)/*.cpp)
TESTS_OBJS    := $(TESTS_SRCS:%.cpp=%.o)
TESTS_TARGETS := $(TESTS_SRCS:$(TESTS_SRC_DIR)/%.cpp=%)

TARGET_DIR       := build
TARGET_DIR_GUARD := @mkdir -p $(TARGET_DIR)

TESTS_TARGET_DIR       := build/tests
TESTS_TARGET_DIR_GUARD := @mkdir -p $(TESTS_TARGET_DIR)

.PHONY: all dpu info

all: $(EXAMPLES_TARGETS)
	@echo "example targets $(EXAMPLES_TARGETS)"

$(EXAMPLES_TARGETS): %: $(EXAMPLES_SRC_DIR)/%.o $(HELIB_OBJS) $(OBJS)
	$(TARGET_DIR_GUARD)
	@$(CC) $^ -o $(TARGET_DIR)/$@ $(LDFLAGS) $(LIBS_DPU)
	@echo "CXX <= $<"

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@ $(LIBS_DPU)
	@echo "CXX <= $<"

dpu:
	$(MAKE) -C src/$@

tests: $(TESTS_TARGETS)
	@echo "test targets $(TESTS_TARGETS)"

$(TESTS_TARGETS): %: $(TESTS_SRC_DIR)/%.o $(HELIB_OBJS) $(OBJS)
	$(TESTS_TARGET_DIR_GUARD)
	@$(CC) $^ -o $(TESTS_TARGET_DIR)/$@ $(LDFLAGS) $(LIBS_DPU)
	@echo "CXX <= $<"

.PHONY: clean-all clean clean-helib clean-build

clean-all: clean clean-helib clean-build

clean:
	rm -f $(HELIB_SRC_DIR)/Ctxt.o
	rm -f $(HELIB_SRC_DIR)/PGFFT.o
	rm -f $(EXAMPLES_SRC_DIR)/*.o $(TESTS_SRC_DIR)/*.o $(SRC_DIR)/*.o

clean-helib:
	rm -f $(HELIB_SRC_DIR)/*.o

clean-build:
	rm -f -r $(TARGET_DIR)

info:
	: HElib require NTL version 11.4.3 or higher
	: Compilation flags are 'CFLAGS=$(CFLAGS)'
	:


