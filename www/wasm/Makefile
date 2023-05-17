#
# SPDX-License-Identifier: LGPL-3.0
# Copyright (c) 向阳, all rights reserved.
#

TARGET = mtfmt

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
$(wildcard ./*.c) \
$(wildcard ../../src/*.c) \
$(wildcard ../../src/**/*.c)

# 编译器
CC = emcc
AR = ar

# C defines
C_DEFS =

# C includes
C_INCLUDES =  \
-I../../inc

# Standard
C_STANDARD = --std=c11

ifeq ($(DEBUG), 1)
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) -D_DEBUG -Wall -fdata-sections -ffunction-sections -g -gdwarf-2
else
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
endif

# 依赖文件
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# build all
all: $(OUTPUT_DIR)/$(TARGET).a
	@echo build completed.

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/$(TARGET).a: $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(AR_DISPLAY) $@
	@$(AR) rcs $@ $(OBJECTS)

$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@
