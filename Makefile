#
# SPDX-License-Identifier: LGPL-3.0
# Copyright (c) 向阳, all rights reserved.
#

# ENV:
# MTFMT_BUILD_GCC_PREFIX                gcc的前缀
# GCC_PATH							gcc路径
# MTFMT_BUILD_TARGET_NAME               目标输出的文件名
# MTFMT_BUILD_C_DEFS                    需要额外增加的cdefs
# MTFMT_BUILD_INC_ADDITIONAL_OUT        输出def文件(opt)
# MTFMT_BUILD_DEBUG                     是否以DEBUG构建
# MTFMT_BUILD_ARCH                      体系结构标记
# MTFMT_BUILD_USE_LTO                   使用LTO
# MTFMT_BUILD_COVERAGE                  代码测试覆盖率
# MTFMT_BUILD_WITH_SANITIZER            需要启用的sanitizer

ifdef MTFMT_BUILD_TARGET_NAME
TARGET_NAME = $(MTFMT_BUILD_TARGET_NAME)
else
TARGET_NAME = mtfmt
endif

ifdef MTFMT_BUILD_LIB_EXT
LIB_EXT = $(MTFMT_BUILD_LIB_EXT)
else
LIB_EXT = .a
endif

ifdef MTFMT_BUILD_DYLIB_EXT
DYLIB_EXT = $(MTFMT_BUILD_DYLIB_EXT)
else
DYLIB_EXT = .so
endif

ifdef MTFMT_BUILD_TESTFILE_EXT
EXE_EXT = $(MTFMT_BUILD_TESTFILE_EXT)
else
EXE_EXT = .out
endif

LIB_TARGET = lib$(TARGET_NAME)$(LIB_EXT)

DYLIB_TARGET = lib$(TARGET_NAME)_dy$(DYLIB_EXT)

DYLIB_IMPLIB_TARGET = lib$(TARGET_NAME)_dy$(LIB_EXT)

TEST_TARGET = $(TARGET_NAME)_run$(EXE_EXT)

# 编译时显示的内容
CC_DISPLAY = CC:

# 链接时显示的内容
LD_DISPLAY = Linker output :

# 打包时显示的内容
AR_DISPLAY = AR output :

# 测试时显示的内容
TEST_DISPLAY = Running test :

# 生成覆盖率报告时显示的内容
GCOV_REPORT_DISPLAY = Gernerate code coverage report ...

# Build path
BUILD_DIR = build

# Test path
TEST_DIR = tests

# 构建输出
ifdef MTFMT_BUILD_OUTPUT_DIR
OUTPUT_DIR = $(MTFMT_BUILD_OUTPUT_DIR)
else
OUTPUT_DIR = target
endif

COVERAGE_DIR = target_coverage

# C sources
C_SOURCES =  \
$(wildcard ./src/*.c) \
$(wildcard ./src/**/*.c)

# 测试源
TEST_C_SOURCES = \
$(wildcard ./tests/*.c) \
$(wildcard ./thirds/unity/src/*.c)

# 测试源 ( C++ )
TEST_CPP_SOURCES= \
$(wildcard ./tests/*.cpp)

# 编译器
ifdef MTFMT_BUILD_GCC_PREFIX
PREFIX = $(MTFMT_BUILD_GCC_PREFIX)
else
PREFIX = 
endif
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AR = $(GCC_PATH)/$(PREFIX)ar
COV = $(GCC_PATH)/$(PREFIX)gcov
else
CC = $(PREFIX)gcc
AR = $(PREFIX)ar
COV = $(GCC_PATH)gcov
endif
COV_REPORT = gcovr

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
# 代码覆盖率选项
ifeq ($(MTFMT_BUILD_COVERAGE),1)
OPT += -fprofile-arcs -ftest-coverage
endif
# sanifizer选项
ifdef MTFMT_BUILD_WITH_SANITIZER
OPT += $(MTFMT_BUILD_WITH_SANITIZER)
endif

# C defines
C_DEFS =

ifdef MTFMT_BUILD_C_DEFS
C_DEFS += $(MTFMT_BUILD_C_DEFS)
endif

# C includes
C_INCLUDES = \
-Iinc \
-Ithirds/unity/src \
-Itests

# Standard
C_STANDARD = --std=c99

# Standard
CXX_STANDARD = --std=c++11

ifeq ($(MTFMT_BUILD_DEBUG), 1)
CFLAGS = $(ARCH) $(C_DEFS) $(C_INCLUDES) $(OPT) -D_DEBUG -Wall -fdata-sections -ffunction-sections -g -gdwarf-2
else
CFLAGS = $(ARCH) $(C_DEFS) $(C_INCLUDES) $(OPT) $(LTO_OPT) -Wall -fdata-sections -ffunction-sections
endif

# C++
# 不使用RTTI
CXX_FLAGS = $(CFLAGS) -fno-rtti --std=c++11

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

# 编译测试文件的链接选项
TEST_LD_OPTS =

ifneq ($(MTFMT_BUILD_DEBUG), 1)
TEST_LD_OPTS += $(LTO_OPT)
endif

TEST_LD_OPTS += -lstdc++
ifeq ($(MTFMT_BUILD_COVERAGE),1)
TEST_LD_OPTS += -lgcov
endif

ifdef MTFMT_BUILD_WITH_SANITIZER
TEST_LD_OPTS += $(MTFMT_BUILD_WITH_SANITIZER)
endif

# 回收不需要的段
DYLIB_LD_OPTS += -Wl,--gc-sections

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# list of objects for tests
TEST_OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(TEST_C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(TEST_C_SOURCES)))

# list of cpp objects
TEST_OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(TEST_CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(TEST_CPP_SOURCES)))

# build all
all: lib dylib test
	@echo Build completed.

# build static lib
lib: $(OUTPUT_DIR)/$(LIB_TARGET)
	@echo Build static library completed.

# build dylib
dylib: $(OUTPUT_DIR)/$(DYLIB_TARGET)
	@echo Link completed.

# 测试
test: $(OUTPUT_DIR)/$(TEST_TARGET)
	@echo $(TEST_DISPLAY) $<
	@"$(addprefix ./$(OUTPUT_DIR)/,$(notdir $<))"

# 测试覆盖率
coverage: test
	@echo Completed.

# 测试覆盖率, 并携带报告
coverage_report: coverage | $(COVERAGE_DIR)
	@echo $(GCOV_REPORT_DISPLAY)
	@$(COV_REPORT) --source-encoding=utf-8 --exclude "test_*" --exclude "thirds/*" --html --html-details --output "$(COVERAGE_DIR)/index.html"
	@echo $(GCOV_REPORT_DISPLAY) completed.

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(C_STANDARD) $(CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR) 
	@echo $(CC_DISPLAY) $<
	@$(CC) -c $(CXX_STANDARD) $(CXX_FLAGS) -MMD -MP -MF"$(@:%.o=%.d)" $< -o $@

$(OUTPUT_DIR)/$(LIB_TARGET): $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(AR_DISPLAY) $@
	@$(AR) rcs $@ $(OBJECTS)

$(OUTPUT_DIR)/$(DYLIB_TARGET): $(OBJECTS) Makefile | $(OUTPUT_DIR)
	@echo $(LD_DISPLAY) $@
	@$(CC) -shared $(OBJECTS) -o $@ $(DYLIB_LD_OPTS)

$(OUTPUT_DIR)/$(TEST_TARGET): $(OBJECTS) $(TEST_OBJECTS) | $(OUTPUT_DIR)
	@echo $(LD_DISPLAY) $@
	@gcc $(OBJECTS) $(TEST_OBJECTS) $(TEST_LD_OPTS) -o $@

$(BUILD_DIR):
	mkdir $@

$(OUTPUT_DIR):
	mkdir $@

$(COVERAGE_DIR):
	mkdir $@

# 依赖
-include $(wildcard $(BUILD_DIR)/*.d)
