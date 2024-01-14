
DPU_SOURCES := $(wildcard *.c)
DPU_TARGETS := $(DPU_SOURCES:%.c=%)

TARGET_DIR       := ../../../build/dpu
TARGET_DIR_GUARD := @mkdir -p $(TARGET_DIR)

INCLUDE := -I../../../include

.PHONY: all clean

all: $(DPU_TARGETS)

%: %.c
	$(TARGET_DIR_GUARD)
	dpu-upmem-dpurte-clang $(INCLUDE) -DSTACK_SIZE_DEFAULT=6144 -DNR_TASKLETS=1 -o $(TARGET_DIR)/$@ $<

clean:
	rm -f -r $(TARGET_DIR)
