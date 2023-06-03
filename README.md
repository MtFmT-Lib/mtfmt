MtFmT
=====

![AUTHOR](https://img.shields.io/badge/Author-XiangYang%20%26%20HalfSweet-blue)
![LICENSE](https://img.shields.io/badge/License-LGPL--V3-green)
![LANGUAGE](https://img.shields.io/badge/Language-100%25%20C-yellowgreen)
[![Coverage Status](https://coveralls.io/repos/github/MtFmT-Lib/mtfmt/badge.svg?branch=better_test)](https://coveralls.io/github/MtFmT-Lib/mtfmt?branch=better_test)


[![DOCUMENT](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/document.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/document.yml)
[![DOCUMENT LINT](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/eslint.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/eslint.yml)
[![TESTS](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/tests.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/tests.yml)
[![DYLIB-BUILD](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-dylib.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-dylib.yml)
[![LIB-BUILD](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-staticlib.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-staticlib.yml)
[![CLANG-FORMAT](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/format-checker.yml/badge.svg)](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/format-checker.yml)


\[ [HomePage](https://mtfmt-lib.github.io/mtfmt/) \]
\[ [Document](https://mtfmt-lib.github.io/mtfmt/doc) \]
\[ [Code Document](https://mtfmt-lib.github.io/mtfmt/doxygen/html/) \]
\[ [Pre-build Download](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-staticlib.yml) \]
\[ [Pre-build DLLs](https://github.com/MtFmT-Lib/mtfmt/actions/workflows/build-dylib.yml) \]


一个类似于`{fmtlib}`和python的format语法的字符串格式化库，以足够低的资源占用和足够高的性能实现string format。
`mtfmt` （Mini template formatter）是为嵌入式系统设计的格式化库，它实现了 [PEP-3101](https://peps.python.org/pep-3101/) 中的格式化串的一个方言。其它类似的实现包括 [{fmtlib}](https://fmt.dev/latest/syntax.html) 等。另外，mtfmt 还实现了.Net中对于日期和时间格式化中的[标准部分](https://learn.microsoft.com/zh-cn/dotnet/standard/base-types/standard-date-and-time-format-strings)、[用户定义部分](https://learn.microsoft.com/zh-cn/dotnet/standard/base-types/custom-date-and-time-format-strings)的一个子集，mtfmt的其主要特性包括：

* [x] 无除法运算、取模运算的有符号/无符号整数格式化（二进制、八进制、十进制、十六进制）
* [x] 无除法运算、取模运算的有符号/无符号的量化值格式化（q31_t等）
* [ ] ~~无除法运算、取模运算的有符号/无符号定点数格式化~~  **懒**
* [x] 标准日期时间格式化、指定分隔符、项位置的日期和时间格式化
* [ ] 12小时制
* [ ] 星期名称
* [x] 字符串格式化
* [x] 指定对齐方式、填充宽度、符号显示方式
* [x] C数组格式化
* [x] 不依赖操作系统，提供用于嵌入式设备的内存分配器
* [x] 提供动态长度字符串，字符串携带 stack allocator
* [x] 提供可传入 `va_list` 函数，变参函数
* [X] 定位parser错误位置以及原因

请注意区分于大多数的格式化器，`MtFmt-Core` **暂不提供**如下的功能：

* 自定义格式化标记
* 自定义错误处理

## 工作方式

MtFmt 通过数个 pass 来完成格式化工作，其均是动态的。

## 详细描述

这里给出了 `mtfmt` 的格式化语法细节。其使用一个小可爱语言来完成它，姑且称作 `MtFmt-DSL` 。MtFmt 通过 `replacement_field` 区分需要放置的值类型，使用 `{` 和 `}` 包起来。对于 replacement field，其至少需要指定参数位置以及参数类型，且参数位置必须是递增的。若使用 `{ A }` 表示0到多个的A, `[ A ]` 表示可选的A，`{ A }+` 表示1到多个的A，`{ A }?` 表示0或1个A。使用不同的行或者“\|”以表示“或”，使用缩进区分符号。下面给出了总体语法。

```plaintext
replacement_field :=
    `{` arg_id `:` arg_type [`:` format_spec] `}`
    `{[` arg_id `:` arg_type  [`|:` split_ch] [`:` format_spec] `]}`

split_ch :=
    split_ch_content
    split_ch split_ch_content

split_ch_content :=
    any characters other than `:`, `}`, or `]}`

format_spec :=
    [ [fill] align] [sign] [width] [format_type | chrono_spec]

chrono_spec :=
    standard_chrono
    user_def_chrono
```

### arg_id: 参数位置

```plaintext
arg_id :=
    { digit }+

digit :=
    one of
        0 1 2 3 4 5 6 7 8 9
```

参数位置指定当前的 replacement field 需要填充第几个参数，需要注意的是，参数位置是递增的，例如，第一个和第二个例子是合法的arg_id，而第三个不是：

```plaintext
"{0:u32} + {1:u32}"
"{0:u32} + {1:u32} + {0:u32}"
"{1:u32} + {0:u32}"
```

参数位置可以是任意个数的整数值。

### arg_type: 参数类型

```plaintext
arg_type :=
    one of
        i8  i16  i32
        u8  u16  u32
        q { digit }+ [ `u` ]
        F { digit }+ [ `.` { digit }+ ] [ `u` ]
        s
        c
        t
```

所有预置的类型如下：

| 预置类型 | 描述           | 示例                                                                     |
| -------- | -------------- | ------------------------------------------------------------------------ |
| i8       | 8位有符号整数  |                                                                          |
| i16      | 16位有符号整数 |                                                                          |
| i32      | 32位有符号整数 |                                                                          |
| u8       | 8位无符号整数  |                                                                          |
| u16      | 16位无符号整数 |                                                                          |
| u32      | 32位无符号整数 |                                                                          |
| qXX      | 量化值         | `q12` 表示 12 位量化值，`q12u` 表示12位无符号量化值                  |
| FXX.XXX  | 定点数         | `F12.4` 表示 12 位整数，4位小数的定点值，`F12.4`u 表示相应的无符号值 |
| s        | C字符串指针    |                                                                          |
| t        | 时间和日期     |                                                                          |

### fill,align: 填充与对齐

```plaintext
fill :=
    a character other than `]}`, `|`, or `}`

align :=
    one of
        < = >
```

填充与对齐是可选部分，其指定填充字符和其中内容的对齐方式。需要注意的是，其必须在 `width` 项被指定的时候才有效。否则由于 `width` 是默认值，不会造成任何效果。填充项是单个的字符，但是其不能是 `]`、`|`、`}`；目前支持的对齐项如下表：

| 对齐选项 | 描述   | 示例                              | 示例结果     |
| -------- | ------ | --------------------------------- | ------------ |
| <        | 左对齐 | `format("{0:i32:#<8}", ..., 1)` | `1#######` |
| =        | 居中   | `format("{0:i32:#=8}", ..., 1)` | `###1####` |
| >        | 右对齐 | `format("{0:i32:#>8}", ..., 1)` | `#######1` |

### sign: 符号显示

```plaintext
sign :=
    one of
        +   -   ` `
```

符号项指定平凡值类型（包括 `u8`、`u16`、`u32`、`i8`、`i16`、`i32`）、量化值 `qXX`、定点数 `FXX.XX`的符号显示方式，对于其余类型无效。目前支持的值符号显示选项如下：

| 符号选项  | 描述                              | 示例                                            | 示例结果    |
| --------- | --------------------------------- | ----------------------------------------------- | ----------- |
| +         | 始终显示符号                      | `format("{0:i32:+},{1:i32:+}", ..., 1, -1)`   | `+1,-1`   |
| -         | 仅在值小于0时显示符号             | `format("{0:i32:-},{1:i32:-}", ..., 1, -1)`   | `1,-1`    |
| ASCII空格 | 在值小于0时显示符号，否则显示空格 | `format("%{0:i32: },{1:i32: }%", ..., 1, -1)` | `% 1,-1%` |

### width: 项宽度

```plaintext
width :=
    { digit }+
```

项宽度指定一个最小值，其是任意一个整数值。

### format_type: 值格式化标记

```plaintext
format_type :=
    one of
        b   d   o   h   H   x   X
```

值格式化标记指定了平凡值类型的格式化方式。所有项的含义如下：

| 格式化标记 | 描述                              | 示例                             | 示例结果 |
| ---------- | --------------------------------- | -------------------------------- | -------- |
| b          | 二进制值                          | `format("{0:i32:b}", ..., 10)` | `1010` |
| d          | 十进制值（默认）                  | `format("{0:i32:d}", ..., 10)` | `10`   |
| o          | 八进制值                          | `format("{0:i32:o}", ..., 10)` | `12`   |
| h          | 十六进制值                        | `format("{0:i32:h}", ..., 10)` | `a`    |
| H          | 十六进制值（大写）                | `format("{0:i32:H}", ..., 10)` | `A`    |
| x          | 十六进制值（带 `0x`前缀）       | `format("{0:i32:x}", ..., 10)` | `0xa`  |
| X          | 十六进制值（大写，带 `0X`前缀） | `format("{0:i32:X}", ..., 10)` | `0XA`  |

### standard_chrono: 标准日期时间格式化项

```plaintext
standard_chrono :=
    one of
        `%` f
        `%` g
```

标准日期格式化项提供了两个预置的格式化方式，其列表如下：

| 选项 | 描述     | 描述字符串                             | 示例                        | 示例结果                       |
| ---- | -------- | -------------------------------------- | --------------------------- | ------------------------------ |
| f    | 完整日期 | `%yyyy-%MM-%dd %HH:%mm:%ss.%xxxx %w` | `format("{0:t:%f}", ...)` | `1919-05-04 14:01:02.1234 7` |
| g    | 标准日期 | `%yyyy-%MM-%dd %HH:%mm:%ss.%xxxx`    | `format("{0:t:%g}", ...)` | `1919-05-04 14:01:02.1234`   |

为什么星期显示不是 `Sunday`呢，因为**懒** (●'◡'●) 。

### user_def_chrono: 自定义日期时间格式化项

```plaintext
user_def_chrono :=
    `%` chrono_fromat_type
    `%` chrono_fromat_type chrono_split_string user_def_chrono
```

自定义日期时间格式化项包括指定如何格式化每个项目和如何指定项之间的分割两个部分。

#### chrono_type: 项格式化

```plaintext
chrono_fromat_type :=
    one of
        y   yy   yyy   yyyy   yyyyy
        M   MM
        d   dd
        h   hh
        H   HH
        m   mm
        s   ss
        x   xx   xxx   xxxx
        w
```

项格式化支持如下内容，其包括年月日时分秒，子秒，星期几个部分，所有支持的项如下：

| 值   | 描述               | 示例                                           | 示例结果 | 解释                 |
| ---- | ------------------ | ---------------------------------------------- | -------- | -------------------- |
| y    | 1~2位的年份        | `format("{0:t:%y}", 1901-01-01...)`          | 1        | 1901（最后1位）      |
| yy   | 固定2位的年份      | `format("{0:t:%yy}", 1901-01-01...)`         | 01       | 1901（最后2位，补0） |
| yyy  | 3~4位年份          | `format("{0:t:%yyy}", 224-01-01...)`         | 224      |                      |
| yyyy | 固定4位年份        | `format("{0:t:%yyy}", 224-01-01...)`         | 0224     | 4位，进行补0         |
| M    | 1~2位月份          | `format("{0:t:%yyy}", 224-01-01...)`         | 1        |                      |
| MM   | 固定2位月份        | `format("{0:t:%yyy}", 224-01-01...)`         | 01       |                      |
| d    | 1~2位天数          | `format("{0:t:%yyy}", 224-01-01...)`         | 1        |                      |
| dd   | 固定2位天数        | `format("{0:t:%yyy}", 224-01-01...)`         | 01       |                      |
| h    | 1~2位的12h制小时   | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 1        |                      |
| hh   | 固定2位的12h制小时 | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 01       |                      |
| H    | 1~2位的24h制小时   | `format("{0:t:%yyy}", ...03:14:52:01314...)` | 3        |                      |
| HH   | 固定2位的24h制小时 | `format("{0:t:%yyy}", ...03:14:52:01314...)` | 03       |                      |
| m    | 1~2位的分钟        | `format("{0:t:%yyy}", ...13:04:52:01314...)` | 4        |                      |
| mm   | 固定2位的分钟      | `format("{0:t:%yyy}", ...13:04:52:01314...)` | 04       |                      |
| s    | 1~2位的秒钟        | `format("{0:t:%yyy}", ...13:14:02:01314...)` | 2        |                      |
| ss   | 固定2位的秒钟      | `format("{0:t:%yyy}", ...13:14:02:01314...)` | 02       |                      |
| x    | 固定1位的亚秒值    | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 0        |                      |
| xx   | 固定2位的亚秒值    | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 01       |                      |
| xxx  | 固定3位的亚秒值    | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 013      |                      |
| xxxx | 固定4位的亚秒值    | `format("{0:t:%yyy}", ...13:14:52:01314...)` | 0131     |                      |
| w    | 星期值             | `format("{0:t:%y}", 1919-05-04...)`          | 7        |                      |

#### chrono_split: 项分割

```plaintext
chrono_split_string :=
    none, or one and more chrono_split_char

chrono_split_char :=
    any character other than `:`, `}`, or `]}`
    `%%`
```

项分割可以是除去 `:`、`}`、`]}` 以外的内容，且可以使用两个连续的 `%` 来表示 `%`

### 数组格式化

TODO

## 例子

TODO

## See also

TODO


