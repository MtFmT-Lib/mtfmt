cmake_minimum_required(VERSION 3.10)

project(mtfmt)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# debug版的后缀
set(CMAKE_DEBUG_POSTFIX "_d")

# 构建输出
set(BUILD_OUTPUT_DIR ${CMAKE_BINARY_DIR})

# 选项
option(MTFMT_RT_USE_DIV "Use hardware divide instruction" OFF)
option(MTFMT_RT_USE_MALLOC "Use malloc instead build-in heap manager" OFF)
option(MTFMT_RT_USE_STDOUT "Enable stdio support in the library" OFF)
option(MTFMT_RT_USE_ASSERT "Enable assert" OFF)
option(MTFMT_RT_USE_UTF8 "Enable UTF-8 string support" ON)
option(MTFMT_RT_USE_FP16 "Enable 16bits floating number formatting support" OFF)
option(MTFMT_RT_USE_FP32 "Enable 32bits floating number formatting support" OFF)
option(MTFMT_RT_USE_FP64 "Enable 64bits floating number formatting support" OFF)
option(MTFMT_BUILD_SHARED "Build dymatic library" OFF)

# 库文件输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DIR}/target/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIR}/target/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUTPUT_DIR}/target/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUTPUT_DIR}/target/bin)

# 源文件
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SRCS)

# 资源文件
if(MSVC AND WIN32)
    set(LIB_WIN_RC "${CMAKE_CURRENT_SOURCE_DIR}/src/win_resource.rc")
else()
    set(LIB_WIN_RC "")
endif()

# 目标名
set(TARGET_NAME mtfmt)

# 构建目标
if(MTFMT_BUILD_SHARED)
    add_library(${TARGET_NAME} SHARED ${LIB_SRCS} ${LIB_WIN_RC})
else()
    add_library(${TARGET_NAME} STATIC ${LIB_SRCS})
endif()

# including path
target_include_directories(${TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/inc")

# debug?
if(NOT MSVC)
    if(NOT CMAKE_BUILD_TYPE)
        message("NO build type was set, the default value is release")
        set(CMAKE_BUILD_TYPE Release)
    endif()
else()
    # 默认release ...
    set(CMAKE_BUILD_TYPE Release)
endif()

# 宏开关
if(MTFMT_RT_USE_DIV)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_HARDWARE_DIV)
endif()

if(MTFMT_RT_USE_MALLOC)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_MALLOC)
endif()

if(MTFMT_RT_USE_STDOUT)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_STD_IO)
endif()

if(MTFMT_RT_USE_ASSERT)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_RUNTIME_ASSERT)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_RUNTIME_CTRLFLOW_MARKER)
endif()

if(MTFMT_RT_USE_UTF8)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_UTF_8)
endif()

if(MTFMT_RT_USE_FP16)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_FP_FLOAT16)
endif()

if(MTFMT_RT_USE_FP32)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_FP_FLOAT32)
endif()

if(MTFMT_RT_USE_FP64)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_USE_FP_FLOAT64)
endif()

if(MSVC)
    # 大多数东东没有second api
    # 因此mtfmt开发过程也没有用它们, disable掉
    target_compile_definitions(${TARGET_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${TARGET_NAME} PRIVATE _DEBUG)
endif()

# dll export
if(MTFMT_BUILD_SHARED)
    target_compile_definitions(${TARGET_NAME} PRIVATE _MSTR_BUILD_DLL)
endif()

install(TARGETS mtfmt
        EXPORT mtfmt-targets
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        PUBLIC_HEADER DESTINATION include()  # 头文件安装路径
        )

# 导出 CMake 配置文件
install(EXPORT mtfmt-targets
    FILE mtfmt-config.cmake
    NAMESPACE mtfmt::
    DESTINATION lib/cmake/mtfmt)

# 测试
# enable_testing()

# file(GLOB_RECURSE TEST_SOURCES tests/*.c thirds/unity/src/*.c)

# foreach(FILE_PATH ${TEST_SOURCES})
# get_filename_component(FILE_NAME ${FILE_PATH} NAME_WE)
# set(TEST_TARGET_NAME ${FILE_NAME}_test)
# add_executable(${TEST_TARGET_NAME} ${FILE_PATH})
# target_include_directories(${TEST_TARGET_NAME} PRIVATE inc thirds/unity/src)
# target_link_libraries(${TEST_TARGET_NAME} PRIVATE ${LIB_TARGET_NAME})
# if (MTFMT_BUILD_USE_LTO)
# set_target_properties(${TEST_TARGET_NAME} PROPERTIES LINK_FLAGS -flto)
# endif()
# add_test(NAME ${FILE_NAME} COMMAND ${TEST_TARGET_NAME})
# endforeach()

# 自定义目标 alltests
# add_custom_target(alltests COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
