MtFmt is a formatter library. It's another implementation for the dialect of PEP-3101 and written in pure C language and optimized for the embedded system.

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

The minimum support C language standard is `ISO C99` and the C++ wrapper requires `ISO C11`.

# 2 Providing API

Formatting API includes four parts. The first one is the string API, which provides a dynamic string for the library. And the second one is the formatting API, which provides the formatting implements with `va_arg`. The third one is a scanner and the last one is an optional heap manager, it's designed for embedded systems without malloc and RTOS. The full documents of source codes are deployed at [HERE](https://mtfmt-lib.github.io/mtfmt/doxygen/html/ "Doxygen document").

## 2.1 Error handling

The resulting code, or `mstr_result_t` type, is a 32 bits unsigned integer.

## 2.2 String

The string API provides a dynamic string object, including optimizing the short string. The basic struct of that is the following.

```c
struct MString
{
    char* buff;
    usize_t count;
    usize_t cap_size;
    char stack_region[MSTR_STACK_REGION_SIZE];
};
```

As you see, the implementation has four properties mainly. The first one, `buff`, is the pointer of the string memory region. The second one, `count`, is the count of buff. It indicates how many bytes of the buffer. And the third one, `cap_size`, also called capacity, is the amount of space in the underlying character buffer. And with the help of property `stack_region`, the string can be allocated on the stack located in the currently active record. So the string is located in the stack when the `stack_region` is equal for the `buff`. Otherwise, it's located in the heap. The main motivation is assuming that the string usually contains few characters, so it can reduce the heap usage in usual. The following sections descript the export API and the design details of `MString`. For the encoding of the string will be controlled by macro `_MSTR_USE_UTF8`, which determines which encoding (UTF-8 or NOT UTF-8) will be used. See more about string encoding in [section 2.5](#section_2_5).

### 2.2.1 Allocator

The allocator is fixed and can be selected by macro `_MSTR_USE_MALLOC`. For the short string, which length is less than `MSTR_STACK_REGION_SIZE`,  the string will be allocated into the stack region. Otherwise, it will be allocated into the heap. The macro `_MSTR_USE_MALLOC` indicated which allocator should be used. When it's equal to 0, the built-in heap manager will be selected. Otherwise, the allocator is malloc, in `stdlib`. Use the macro functions as follows to replace the malloc in your application. Those will be switched by the macro `_MSTR_USE_MALLOC`. See more details in [section 2.4](#section_2_4).

* `mstr_heap_init`: Initialize the heap
* `mstr_heap_alloc`: Allocate memory
* `mstr_heap_free`: Release

### 2.2.2 Constructor and Destructor

#### 2.2.2.1 Constructor

The construct functions of this object include three different versions. The first version, `mstr_create` is to create a string from a C-style string pointer directly. The second one, `mstr_copy_create` is to copy another string object to create. And the last one, `mstr_move_create` is to move memory from another object to create.

The usage of `mstr_create` is simple and shows follow. After constructing from this function, the result must be called `mstr_free` by hand shown in [section 2.2.2.2](#section_2_2_2_2)

```c
MString str;
mstr_create(&str, "string");
// str == "string"
```

The `mstr_copy_create` is designed for copying another object. The following shows how to copy the source string to the destination string. After constructing from this function, the result must be called `mstr_free` by hand and more details show in [section 2.2.2.2](#section_2_2_2_2)

```c
MString str;
mstr_create(&str, "string");
// str == "string"
MString another;
mstr_copy_create(&str, &another);
// another == "string"
```

The `mstr_move_create` means moving the memory from the other object. So that called `mstr_free`  is not necessary if constructed from this function. The following code shows how to move that.

```c
MString str;
mstr_create(&str, "string");
// str == "string"
MString another;
mstr_move_create(&str, &another);
// another == "string"
// And, str is empty now
```

#### 2.2.2.2 Destruct function

The destruct function will release memory simply like the following code.

```c
MString str;
mstr_create(&str, "string");
// str == "string"
mstr_free(&str);
// str is empty.
```

This function must be called by hand after calling the most of construct function. Otherwise, it will cause a memory leak.

### 2.2.3 Transformation

The transformation functions provide reverse string and clear string. The transformation function has the same input and output, it will be run in situ, and the evaluation result always is `MStr_Ok`. So that has no return value. The prototype of those is the following.

```c
void mstr_TRANS(MString* input_output);
```

As an example, the following code shows how to reverse a string using `mstr_reverse_self`. All transformation functions have the same usage.

```c
MString str;
mstr_create(&str, "Example");
mstr_reverse_self(&str);
// now, str == 'elpmaxE'
```

#### 2.2.3.1 Clear string

The clear function `mstr_clear` sets the `length` property to zero simply. It will make this object seem empty after this function is called.

For a string that holds length $N$, this function implementation has $O(1)$ time complexity and spatial complexity.

#### 2.2.3.1 Reverse string

The reverse function `mstr_reverse_self` reverses all characters in the original string. This function will reverse one by one character rather than just reverse byte.

For a string that holds length $N$, this function implementation has $\Theta(N)$ time complexity and $O(1)$ spatial complexity.

### 2.2.4 Concatenation

The concatenating operator for string means that push one or more characters into the source string. The library provides three functions as follows.

* Pushing functions: `mstr_append` and `mstr_repeat_append`
* Concatenating functions: `mstr_concat` , `mstr_concat_cstr_slice` and `mstr_concat_cstr`

#### 2.2.4.1 Push

The push function means append one character to the string. For example, the following code shows how to use `mstr_append` and the result.

```c
MString str;
mstr_create(&str, "Example");
mstr_append(&str, '@');
// now, str == 'Example@'
```

The `mstr_repeat_append` is a specific version for the `mstr_append`, which means repeat push $N$ characters. In the implementation, the string has a property called `cap_size`, that determined the allocated memory size of this object. The usage of memory will increase when continuing to push the character into that. The increasing step is fixed or selected by some tactics. So that it will trigger heap allocating once and once, causing performance degradation. A suitable solution is that allocated enough memory before operating. The function  `mstr_repeat_append` includes a parameter that indicates the count of the input characters that make specifying the count of characters feasible. Here is an example that shows how to push three `#` characters.

```c
MString str;
mstr_create(&str, "Example");
mstr_repeat_append(&str, '#', 3);
// now, str == 'Example###'
```

For $N$ characters and the $L$ length of a string, both two functions have $\Theta(N)$ time complexity.

#### 2.2.4.2 Concatenation

The concatenating function concatenates the string arguments to the calling string and returns the result code. The library provides three different specific versions.

* `mstr_concat`: concatenate one string to another string
* `mstr_concat_cstr`: concatenate one C-style string pointer to a string object
* `mstr_concat_cstr_slice`: concatenate a C-style string pointer between two addresses to a string object

The following shows how to use those functions.

```c
MString str;
mstr_create(&str, "Example");
// Concatate another string object
MString another;
mstr_create(&another, "-concat");
mstr_concat(&str, &another);
// Now, str is "Example-concat" and the other is "-concat"
const char* c_style_str = "#1#";
mstr_concat_cstr(&str, c_style_str);
// Now, str is "Example-concat#1#"
const char* c_style_point = "0123456789";
const char* begin = c_style_point + 1;
const char* end = begin + 4;
mstr_concat_cstr_slice(&str, begin, end);
// Now, str is "Example-concat#1#1234"
// "1234" -> [begin, end)
```

For the $N$ length source string aka. `str` in the code and the $M$ length right side string, those operators have $\Theta(M)$ time complexity.

### 2.2.5 Comparison operator

The comparison function provides a way to compare two strings. If that is the same this function will return `true` otherwise return `false`. The following code shows how to compare two strings.

#### 2.2.5.1 Compare all

The function `mstr_equal` provides comparing two strings.

```c
MString str1, str2;
mstr_create(&str1, "str1");
mstr_create(&str2, "str2");
bool res = mstr_equal(&str1, &str2);
// res is false.
```

The function holds two properties as follows where $S_1=S_2$ is `mstr_equal(&s1, &s2)` and all vaild strings $S\in\mathcal{S}$

* Commutative: $\forall S_1,S_2\in\mathcal{S}$, then $S_1=S_2$ equal $S_2=S_1$
* Associative: $\forall S_1,S_2,S_3\in\mathcal{S}$, then $S_1=(S_2=S_3)$ equal $(S_1=S_2)=S_3$

For the $N$ length string and an $M$ length string, the comparison function has $\Theta\left(\min\left[N,M\right]\right)$ time complexity.

#### 2.2.5.2 Compare the prefix

The function `mstr_start_with` returns `true` if the given pattern matches a prefix of this string.

```c
MString str;
mstr_create(&str, "Example");
assert(mstr_start_with(&str, "Exa") == true);
assert(mstr_start_with(&str, "Example") == true);
assert(mstr_start_with(&str, "!Example") == false);
```

#### 2.2.5.3 Compare the suffix

The function `mstr_end_with` returns `true` if the given pattern matches a suffix of this string.

```c
MString str;
mstr_create(&str, "Example");
assert(mstr_end_with(&str, "ample") == true);
assert(mstr_end_with(&str, "Example") == true);
assert(mstr_end_with(&str, "ample!") == false);
```

### 2.2.6 Substring matcher

TODO

### 2.2.7 Indexing operators

TODO

### 2.2.8 Other functions

The other functions include the type converts function.

#### 2.2.8.1 Convert to C-style string

The function `mstr_as_cstr` provides a method that returns the c-style string pointer of the original string. Due to deliberate design reasons, the string capacity always contains one byte. That means the `cap_size` is greater than the `length` is true in all time. So that this function will append one byte in which the value is zero if necessary and return the c-style string pointer directly. The following code shows the usage.

```c
MString str;
mstr_create(&str, "string");
// Convert to c-style string
const char* c_str = mstr_as_cstr(&str);
// Now, c_str is pointing to "string" and terminated by '\0'
```

## 2.3 Formatter

The formatter API includes two parts. The first part is the `mstr_format` function, which provides formatting inputs and appends the output into the string with variable arguments. Notes that the function will append the output directly instead clear the string and append results.

### 2.3.1 Formatter function

TODO

### 2.3.2 Error report

TODO

## 2.4 Build-in heap manager

The built-in heap manager is optional and will be compiled when the macro `_MSTR_USE_MALLOC` is zero.

### 2.4.1 Allocator functions

The macro function `mstr_heap_init`

### 2.4.2 Fragment counter

TODO

## 2.5 Advanced topics

TODO

### 2.5.1 Encoding of character

TODO

### 2.5.2 Monadic result

TODO

### 2.5.3 C++ Wrapper

TODO

# 3 Syntax

The syntax is not context-free syntax, but it can be parsed by a top-down parser easily. The parser will match the replacement field for the input of the formatting string. The replacement field describes how to process arguments. Consider the input as follows, it shows how to format `input_tm` into `output`.

```c
mstr_result_t result_code = mstr_format(
    &output,
    "Today is {0:t:%yyyy-%MM-%dd}",
    1,
    &input_tm
);
```

The first argument is a literal string.

## 3.1 Foundations for formal languages

The formal language in this section means a well-defined language. It's not a natural language in the real world. If you know how a parser work and know how to read a formal syntax, you can [skip this section](#section_3_2).

## 3.2 Notational conventions

TODO

# 4 Compile the library

TODO

## 4.1 The Compiler 101

TODO

## 4.2 Dependent files

TODO

# 5 Developing

TODO

# 6 See also

TODO

# Annex A: export functions

This annex includes a summary of all export functions.

# Annex B: macro options

Table 8.1 shows all available macros and the default value.

!table-caption: Table 8.1 available macros

| Macros                     | Default | Description                                           |
| -------------------------- | :-----: | ----------------------------------------------------- |
| `_MSTR_USE_HARDWARE_DIV`   |    0    | Enable to use hardware divider operator.              |
| `_MSTR_USE_MALLOC `        |    0    | Enable to use `malloc` instead build-in heap manager. |
| `_MSTR_RUNTIME_HEAP_ALIGN` |    4    | Specify the alignment of build-in heap manager.       |
| `_MSTR_USE_STD_IO`         |    0    | Enable to use `stdout` support.                       |
| `_MSTR_USE_UTF_8`          |    1    | Enable the UTF-8 support.                             |

# Annex C: syntax summary

TODO

# Annex D: the reference card

TODO
