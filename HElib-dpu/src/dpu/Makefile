
NR_TASKLETS ?= 8

DPU_SOURCES := $(wildcard *.c)
DPU_TARGETS := $(DPU_SOURCES:%.c=%)

TARGET_DIR       := ../../build/dpu
TARGET_DIR_GUARD := @mkdir -p $(TARGET_DIR)

INCLUDE := -I../../include

.PHONY: all single-tasklet clean

all: $(DPU_TARGETS) single-tasklet

%: %.c
	$(TARGET_DIR_GUARD)
	@dpu-upmem-dpurte-clang $(INCLUDE) -DSTACK_SIZE_DEFAULT=6144 -DNR_TASKLETS=$(NR_TASKLETS) -o $(TARGET_DIR)/$@ $<
	@echo "CXX <= $<"

single-tasklet:
	$(MAKE) -C $@

clean:
	rm -f -r $(TARGET_DIR)
