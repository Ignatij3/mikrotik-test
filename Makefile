# enable parallel build and silence commands
MAKEFLAGS += --jobs=32 --silent

# output definitions
BIN_DIR   := bin
BUILD_DIR := build
TARGET    := $(BIN_DIR)/program

TOOLCHAIN := arm-none-eabi
CC        := $(TOOLCHAIN)-gcc
CXX       := $(TOOLCHAIN)-g++
OBJDUMP   := $(TOOLCHAIN)-objdump
OBJCOPY   := $(TOOLCHAIN)-objcopy

# recursive wildcard
# https://blog.jgc.org/2011/07/gnu-make-recursive-wildcard-function.html
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# returns all subdirectories recursively, excluding .cpp and .hpp files
excl_list = *.hpp *.cpp
rgetdirs  = $(foreach dir,$(wildcard $1),$(call rgetdirs,$(dir)/**) $(filter-out $(subst *,%,$(excl_list)),$(dir)))

makedir = mkdir -p "$(1)"

# I don't fully understand how it works, but here's the source: https://stackoverflow.com/a/14447924
INCLUDES := $(call rgetdirs,drivers*) $(call rgetdirs,libs*)
SRCS     := main.cpp startup/startup.c $(call rwildcard,drivers,*.cpp) $(call rwildcard,libs,*.cpp)

CFLAGS   := -std=c23 $(LDFLAGS) $(addprefix -I,$(INCLUDES))
CXXFLAGS := -std=c++23 $(LDFLAGS) $(addprefix -I,$(INCLUDES)) -pedantic-errors -Werror

ARMFLAGS := -mcpu=cortex-m4 -mlittle-endian -mthumb

LDFLAGS  += $(ARMFLAGS) -Tstartup/linker_script.ld -Wl,--gc-sections -Wl,-Map,$(TARGET).map \
  --specs=nano.specs --specs=nosys.specs -nodefaultlibs -nolibc -nostdlib -lgcc

OBJS := \
  $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS))) \
  $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))     \
  $(patsubst %.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRCS)))

# makefile rules
.PHONY: all clean sources
all: $(TARGET).elf $(TARGET).bin

clean:
	rm -rf $(BIN_DIR) 2>/dev/null || true
	rm -rf $(BUILD_DIR) 2>/dev/null || true

sources:
	$(info  SOURCES  = $(SRCS))
	$(info  INCLUDES = $(INCLUDES))
	$(info  OBJECTS  = $(OBJS))

# compile sources
$(BUILD_DIR)/%.o: %.c
	$(call makedir,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(call makedir,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	$(call makedir,$(dir $@))
	$(CC) $(ARMFLAGS) -c $< -o $@

# link objects together
$(TARGET).elf: $(OBJS)
	$(call makedir,$(dir $@))
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@