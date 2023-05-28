#
# SPDX-License-Identifier: LGPL-3.0
# Copyright (c) 向阳, all rights reserved.
#

# ENV:
# MTFMT_BUILD_GCC_PREFIX 			gcc的前缀
# GCC_PATH							gcc路径
# MTFMT_BUILD_TARGET_NAME			目标输出的文件名
# MTFMT_BUILD_C_DEFS				需要额外增加的cdefs
# MTFMT_BUILD_INC_ADDITIONAL_OUT	输出def文件(opt)
# MTFMT_BUILD_DEBUG					是否以DEBUG构建
# MTFMT_BUILD_ARCH					体系结构标记
# MTFMT_BUILD_USE_LTO				使用LTO

ifdef MTFMT_BUILD_TARGET_NAME
TARGET_NAME = $(MTFMT_BUILD_TARGET_NAME)
else
TARGET_NAME = mtfmt
endif

DYLIB_EXT = .so

LIB_TARGET = lib$(TARGET_NAME).a

DYLIB_TARGET = $(TARGET_NAME)_dll$(DYLIB_EXT)

DYLIB_IMPLIB_TARGET = $(TARGET_NAME)_dll.a

# 编译时显示的内容
CC_DISPLAY = CC:

# 链接时显示的内容
LD_DISPLAY = Linker output :

# 打包时显示的内容
AR_DISPLAY = AR output :

# Table generator
TBGEN_DISPLAY = TB:

# Build path
BUILD_DIR = build

# 构建输出
OUTPUT_DIR = target

# C sources
C_SOURCES =  \
$(wildcard ./src/*.c) \
$(wildcard ./src/**/*.c)

# 编译器
ifdef MTFMT_BUILD_GCC_PREFIX
PREFIX = $(MTFMT_BUILD_GCC_PREFIX)
else
PREFIX = 
endif
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AR = $(GCC_PATH)/$(PREFIX)ar
else
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
endif

# arch opt
ifdef MTFMT_BUILD_ARCH
ARCH = $(MTFMT_BUILD_ARCH)
else
ARCH = 
# e.g., -mcpu=cortex-m0plus -mthumb
endif

# LTO
ifdef MTFMT_BUILD_USE_LTO
LTO_OPT = -flto
else
LTO_OPT =
endif

# 选项
OPT = -fpic

# C defines
C_DEFS =

ifdef MTFMT_BUILD_C_DEFS
C_DEFS += $(MTFMT_BUILD_C_DEFS)
endif

# C includes
C_INCLUDES =  \
-Iinc

# Standard
C_STANDARD = --std=c11

ifeq ($(MTFMT_BUILD_DEBUG), 1)
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) -D_DEBUG -Wall -fdata-sections -ffunction-sections -g -gdwarf-2
else
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) $(LTO_OPT) -Wall -fdata-sections -ffunction-sections
endif

# 动态链接库的链接选项
DYLIB_LD_OPTS =	

ifneq ($(MTFMT_BUILD_DEBUG), 1)
DYLIB_LD_OPTS += $(LTO_OPT)
endif

# 回收不需要的段
DYLIB_LD_OPTS += -Wl,--gc-sections

ifeq ($(MTFMT_BUILD_INC_ADDITIONAL_OUT), 1)
DYLIB_LD_OPTS += -Wl,--output-def,$(OUTPUT_DIR)/$(TARGET_NAME).def,--out-implib,$(OUTPUT_DIR)/$(DYLIB_IMPLIB_TARGET)
endif

# 依赖文件
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# build all
all: $(OUTPUT_DIR)/$(LIB_TARGET) $(OUTPUT_DIR)/$(DYLIB_TARGET)
	@echo build completed.

# build static lib
lib: $(OUTPUT_DIR)/$(LIB_TARGET)
	@echo build static library completed.

# build dylib
dylib: $(OUTPUT_DIR)/$(DYLIB_TARGET)
	@echo link completed.

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/$(LIB_TARGET): $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(AR_DISPLAY) $@
	@$(AR) rcs $@ $(OBJECTS)

$(OUTPUT_DIR)/$(DYLIB_TARGET): $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(LD_DISPLAY) $@
	@$(CC) -shared $(OBJECTS) -o $@ $(DYLIB_LD_OPTS)

$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@

# 依赖
-include $(wildcard $(BUILD_DIR)/*.d)
