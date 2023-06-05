cmake_minimum_required(VERSION 3.0)

project(MtFmT)

set(CMAKE_C_STANDARD 11)

# 选项
option(MTFMT_BUILD_DEBUG "设置为 ON 以构建调试版本" OFF)
option(MTFMT_BUILD_USE_LTO "设置为 ON 以启用链接时优化（LTO）" OFF)
option(MTFMT_BUILD_INC_ADDITIONAL_OUT "设置为 ON 以生成动态库的额外输出文件" OFF)

if(MTFMT_BUILD_TARGET_NAME)
    set(TARGET_NAME ${MTFMT_BUILD_TARGET_NAME})
else()
    set(TARGET_NAME "mtfmt")
endif()

if(MTFMT_BUILD_LIB_EXT)
    set(LIB_EXT ${MTFMT_BUILD_LIB_EXT})
else()
    set(LIB_EXT ".a")
endif()

if(MTFMT_BUILD_DYLIB_EXT)
    set(DYLIB_EXT ${MTFMT_BUILD_DYLIB_EXT})
else()
    set(DYLIB_EXT ".so")
endif()

set(LIB_TARGET "lib${TARGET_NAME}${LIB_EXT}")

set(DYLIB_TARGET "lib${TARGET_NAME}_dy${DYLIB_EXT}")

set(DYLIB_IMPLIB_TARGET "lib${TARGET_NAME}_dy${LIB_EXT}")

# 构建输出
if(MTFMT_BUILD_OUTPUT_DIR)
    set(OUTPUT_DIR ${MTFMT_BUILD_OUTPUT_DIR})
else()
    set(OUTPUT_DIR "target")
endif()

# 源文件
file(GLOB_RECURSE SOURCES src/*.c)

# 宏定义
add_compile_definitions(${MTFMT_BUILD_C_DEFS})

if(MTFMT_BUILD_ARCH)
    set(ARCH ${MTFMT_BUILD_ARCH})
else()
    set(ARCH "")
endif()

if(MTFMT_BUILD_USE_LTO)
    set(LTO "-flto")
else()
    set(LTO "")
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # -fPic这个编译选项是GCC独有的，因此如果是别的平台无需打开
    set(OPT "-fpic")

    set(GUN_C_FLAGS "-fdata-sections -ffunction-sections")
else()
    set(OPT "")
    set(GUN_C_FLAGS "")
endif()

# C defines
if (MTFMT_BUILD_C_DEFS)
    set(C_DEFS "${MTFMT_BUILD_C_DEFS}")
else()
    set(C_DEFS "")
endif()

# 包含目录
include_directories(inc)

# 编译器选项
if (MTFMT_BUILD_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH} ${C_STANDARD} ${C_DEFS} ${C_INCLUDES} ${OPT} ${GUN_C_FLAGS} -Wall -D_DEBUG -g -gdwarf-2")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH} ${C_STANDARD} ${C_DEFS} ${C_INCLUDES} ${OPT} ${LTO_OPT}  ${GUN_C_FLAGS} -Wall")
endif()

if(NOT MTFMT_BUILD_DEBUG)
    set(DYLIB_LD_OPTS "${DYLIB_LD_OPTS} ${LTO_OPT}")
endif()

set(DYLIB_LD_OPTS "${DYLIB_LD_OPTS} -Wl,--gc-sections")

if(NOT MTFMT_BUILD_INC_ADDITIONAL_OUT)
    set(DYLIB_LD_OPTS "${DYLIB_LD_OPTS} -Wl,--output-def,${OUTPUT_DIR}/${TARGET_NAME}.def,--out-implib,${OUTPUT_DIR}/${DYLIB_IMPLIB_TARGET}")
endif()

# 构建目录
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${BUILD_DIR})

# 静态库
set(LIB_TARGET_NAME mtfmt)

add_library(${LIB_TARGET_NAME} STATIC ${SOURCES})
set_target_properties(${LIB_TARGET_NAME} PROPERTIES OUTPUT_NAME ${LIB_TARGET_NAME})
target_include_directories(${LIB_TARGET_NAME} PUBLIC inc)

# 动态库

if (WIN32)
    set(DYLIB_EXT .dll)
else()
    set(DYLIB_EXT .so)
endif()

set(DYLIB_IMPLIB_TARGET lib${LIB_TARGET_NAME}_dy${DYLIB_EXT})

set(DYLIB_TARGET_NAME ${LIB_TARGET_NAME}_dy)

add_library(${DYLIB_TARGET_NAME} SHARED ${SOURCES})
set_target_properties(${DYLIB_TARGET_NAME} PROPERTIES OUTPUT_NAME ${DYLIB_TARGET_NAME})
target_include_directories(${DYLIB_TARGET_NAME} PUBLIC inc)

if (MTFMT_BUILD_DEBUG)
    target_compile_definitions(${DYLIB_TARGET_NAME} PRIVATE _DEBUG)
endif()

# 对于非 Windows 平台，设置动态库的额外链接选项
if (NOT WIN32)
    set(DYLIB_LINK_FLAGS -Wl,--gc-sections)
    if (MTFMT_BUILD_INC_ADDITIONAL_OUT)
        set(DYLIB_LINK_FLAGS ${DYLIB_LINK_FLAGS} -Wl,--output-def,${CMAKE_CURRENT_BINARY_DIR}/${LIB_TARGET_NAME}.def,--out-implib,${CMAKE_CURRENT_BINARY_DIR}/${DYLIB_IMPLIB_TARGET})
    endif()
    target_link_options(${DYLIB_TARGET_NAME} PRIVATE ${DYLIB_LINK_FLAGS})
endif()

install(TARGETS mtfmt)

# 测试
# enable_testing()

# file(GLOB_RECURSE TEST_SOURCES tests/*.c thirds/unity/src/*.c)

# foreach(FILE_PATH ${TEST_SOURCES})
#     get_filename_component(FILE_NAME ${FILE_PATH} NAME_WE)
#     set(TEST_TARGET_NAME ${FILE_NAME}_test)
#     add_executable(${TEST_TARGET_NAME} ${FILE_PATH})
#     target_include_directories(${TEST_TARGET_NAME} PRIVATE inc thirds/unity/src)
#     target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${LIB_TARGET_NAME})
#     if (MTFMT_BUILD_USE_LTO)
#         set_target_properties(${TEST_TARGET_NAME} PROPERTIES LINK_FLAGS -flto)
#     endif()
#     add_test(NAME ${FILE_NAME} COMMAND ${TEST_TARGET_NAME})
# endforeach()

# 自定义目标 alltests
# add_custom_target(alltests COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

