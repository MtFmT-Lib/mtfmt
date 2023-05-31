Mtfmt is a formatter library. It's another implementation for the dialect of PEP-3101 and written in pure C language and optimized for the embedded system.

[toc]

# 1 Introduction

The mini template formatting library, or MtFmt is an effective, small formatter library written in pure C language. It implements a dialect for [PEP-3101](https://peps.python.org/pep-3101/ "PEP-3101"). In addition, it implements fixed-point number and quantized value formatting without the divide operator required and implements real-time formatting referencing the [.Net standard](https://learn.microsoft.com/zh-cn/dotnet/standard/base-types/standard-date-and-time-format-strings "Standard time formatting"). The main features are the following.

* Integer formatting supports binary, octal, decimal hexadecimal, and hexadecimal with prefix output
* Quantized value formatting
* Standard time formatting and user-defined time formatting
* String formatting
* Array formatting
* Specifiable align, the filling width, fill character, and sign display style
* Optional build-in heap manager for embedded system
* Optional specific build-in divide operator implementing
* Flexible syntax and clear error report.

# 2 Providing API

Formatting API includes four parts. The first one is the string API, which provides a dynamic string for the library. And the second one is the formatting API, which provides the formatting implements with `va_arg`. The third one is a scanner and the last one is an optional heap manager, it's designed for embedded systems without malloc and RTOS. The full documents of source codes are deployed at [HERE](https://mtfmt-lib.github.io/mtfmt/doxygen/html/ "Doxygen document").

## 2.1 Error handling

The result code, or `mstr_result_t` type, is a 32 bits unsigned integer.

## 2.2 String

The string API provides a dynamic string object, including optimizing the short string. The basic struct of that is the following.

```c
struct MString
{
    char* buff;
    usize_t length;
    usize_t cap_size;
    char stack_region[MSTR_STACK_REGION_SIZE];
};
```

As you see, the implementation has four properties mainly. The first one, `buff`, is the pointer of the string memory region. The second one, `length`, is the count of buff. It indicates how many bytes of the buffer. And the third one, `cap_size`, also called capacity, is the amount of space in the underlying character buffer. And with the help of property `stack_region`, the string can be allocated on the stack located in the currently active record. So the string is located in the stack when the `stack_region` is equal for the `buff`. Otherwise, it's located in the heap. The main motivation is assuming that the string usually contains few characters, so it can reduce the heap usage in usual. The following sections descript the export API and the design details of `MString`.

### 2.2.1 Allocator

The allocator is fixed and can be selected by macro `_MSTR_USE_MALLOC`. For the short string, which length is less than `MSTR_STACK_REGION_SIZE`,  the string will be allocated into the stack region. Otherwise, it will be allocated into the heap. The macro `_MSTR_USE_MALLOC` indicated which allocator should be used. When it's equal to 0, the built-in heap manager will be selected. Otherwise, the allocator is malloc, in `stdlib`. Normally, use the macro functions as follows to replace the malloc in your application. Those will be switched by the macro `_MSTR_USE_MALLOC`. See more details in [section 2.5](#section_2_5_Build_in_heap_manager).

* `mstr_heap_init`: Initialize the heap
* `mstr_heap_alloc`: Allocate memory
* `mstr_heap_free`: Release

#### 2.2.1.1 Macro: mstr_heap_init

The macro function `mstr_heap_init` 

### 2.2.2 Transformation

TODO

### 2.2.3 Concatenating

The concatenating operator for string means that push one or more characters into the source string. 

### 2.2.4 Equal operator

Equal

### 2.2.5 Other functions

The other

## 2.3 Formatter

The formatter API including two parts. The first parts is `mstr_format` function, which provides formatting inputs and append the output into string with var arguments.

## 2.4 Scanner

TODO

## 2.5 Build-in heap manager

TODO

## 2.6 Advanced topics

TODO

# 3 Syntax

The syntax is NOT context-free syntax.

## 3.1 The formal language 101

The formal language, 

# 4 Compile the library

TODO

## 4.1 The Compiler 101

TODO

# 5 Developing

TODO

# 6 See also

TODO
