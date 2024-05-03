
NR_TASKLETS ?= 8

STACK_SIZE_DEFAULT := 8000
ifeq ($(NR_TASKLETS), 16)
	STACK_SIZE_DEFAULT := 4000
endif

DPU_SOURCES := $(wildcard *.c)
DPU_TARGETS := $(DPU_SOURCES:%.c=%)

TARGET_DIR       := ../../build/dpu
TARGET_DIR_GUARD := @mkdir -p $(TARGET_DIR)

INCLUDE := -I../../include

.PHONY: all single-tasklet clean

all: $(DPU_TARGETS) single-tasklet

%: %.c
	$(TARGET_DIR_GUARD)
	@dpu-upmem-dpurte-clang $(INCLUDE) -DSTACK_SIZE_DEFAULT=$(STACK_SIZE_DEFAULT) -DNR_TASKLETS=$(NR_TASKLETS) -o $(TARGET_DIR)/$@ $<
	@echo "CXX <= $<"

single-tasklet:
	$(MAKE) -C $@

clean:
	rm -f -r $(TARGET_DIR)
