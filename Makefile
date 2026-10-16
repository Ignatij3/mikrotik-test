# enable parallel build and silence commands
MAKEFLAGS += --jobs=32 --silent

BAUDRATE ?= 9600
LED1_PIN ?= B6
LED2_PIN ?= B8

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
INCLUDES := platform $(call rgetdirs,drivers*) $(call rgetdirs,libs*)

SRCS :=                           \
  main.cpp                        \
  $(call rwildcard,platform,*.s)  \
  $(call rwildcard,platform,*.c)  \
  $(call rwildcard,drivers,*.cpp) \
  $(call rwildcard,libs,*.cpp)

ARMFLAGS := -mcpu=cortex-m3 -mlittle-endian -mthumb

CFLAGS   := -std=c23   $(ARMFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDES))

CXXFLAGS := -std=c++23 $(ARMFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDES)) \
  -Werror -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics   \
  -DBAUDRATE=$(BAUDRATE) -DLED1_PIN=$(LED1_PIN) -DLED2_PIN=$(LED2_PIN)

LDFLAGS  += $(ARMFLAGS) -Tplatform/linker_script.ld -Wl,--gc-sections -Wl,-Map,$(TARGET).map \
  --specs=nano.specs --specs=nosys.specs -nodefaultlibs -nolibc -nostdlib -lgcc 

OBJS := \
  $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS))) \
  $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))     \
  $(patsubst %.s,$(BUILD_DIR)/%.o,$(filter %.s,$(SRCS)))

# makefile rules
.PHONY: all clean sources flash
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

flash:
	openocd -d0 -f ./platform/stm32f103c8.cfg -c init -c "reset halt" -c "flash write_image erase $(TARGET).elf" -c "reset" -c exit