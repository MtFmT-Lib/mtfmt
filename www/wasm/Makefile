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

# 需要导出的函数
C_EXPORT_FUNCS = \
"EXPORTED_FUNCTIONS=[\
_mstr_create,_mstr_c_str,_mstr_clear,\
_mstr_concat_cstr,_mstr_free,\
_mstr_format,\
_mstr_configure,\
_mstr_wasm_new_string,_mstr_wasm_free_string,\
_mstr_wasm_string_len,\
_mstr_wasm_string_count,\
_mstr_wasm_allocate,_mstr_wasm_free]"

# 编译器
CC = emcc
AR = ar

# C defines
C_DEFS = -D_MSTR_USE_MALLOC=1

# C includes
C_INCLUDES =  \
-I../../inc

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

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/$(TARGET).a: $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(AR_DISPLAY) $@
	@$(AR) rcs $@ $(OBJECTS)

$(OUTPUT_DIR)/$(TARGET).wasm: $(OUTPUT_DIR)/$(TARGET).a
	@echo $(LD_DISPLAY) $@
	@$(CC) $< -o $@ --no-entry -s STANDALONE_WASM -s WASM=1 -s $(C_EXPORT_FUNCS)

$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@
