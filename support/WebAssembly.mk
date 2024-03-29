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

# 导出函数的文件
EXPORT_FILE = support/WebAssemblyExport.mk

# self
THIS_FILE = support/WebAssembly.mk

# C sources
C_SOURCES =  \
$(wildcard src/*.c) \
$(wildcard src/**/*.c)

# 需要导出的函数
include $(EXPORT_FILE)
C_EXPORT_FUNCS = "EXPORTED_FUNCTIONS=[$(EXPORTS)]"

# 编译器
CC = emcc
AR = emar

# C defines
C_DEFS = -D_MSTR_USE_MALLOC=1

# C includes
C_INCLUDES =  \
-Iinc

# Standard
C_STANDARD = --std=c11

ifeq ($(DEBUG), 1)
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) -D_DEBUG -Wall
else
CFLAGS = $(ARCH) $(C_STANDARD) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall
endif

# 依赖文件
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# build all
all: $(OUTPUT_DIR)/$(TARGET).wasm
	@echo build completed.

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c $(THIS_FILE) | $(BUILD_DIR)
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/$(TARGET).a: $(OBJECTS) $(THIS_FILE) | $(OUTPUT_DIR)
	@echo $(AR_DISPLAY) $@
	@$(AR) rcs $@ $(OBJECTS)

$(OUTPUT_DIR)/$(TARGET).wasm: $(OUTPUT_DIR)/$(TARGET).a
	@echo $(LD_DISPLAY) $@
	@$(CC) $< -o $@ --no-entry -s STANDALONE_WASM -s WASM=1 -s $(C_EXPORT_FUNCS)

$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@

# 依赖
-include $(wildcard $(BUILD_DIR)/*.d)
