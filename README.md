# MtFmT
一个类似于`{fmtlib}`和python的format语法的字符串格式化库，以足够低的资源占用和足够高的性能实现string format。
`mtfmt` （Mini template formatter）是为嵌入式系统设计的格式化库，它实现了 [PEP-3101](https://peps.python.org/pep-3101/) 中的格式化串的一个方言。其它类似的实现包括 [{fmtlib}](https://fmt.dev/latest/syntax.html) 等。另外，mtfmt 还实现了.Net中对于日期和时间格式化中的[标准部分](https://learn.microsoft.com/zh-cn/dotnet/standard/base-types/standard-date-and-time-format-strings)、[用户定义部分](https://learn.microsoft.com/zh-cn/dotnet/standard/base-types/custom-date-and-time-format-strings)的一个子集，且在 `ARM Cortex-M0+` 处理器上核心仅需 8kB 大小的指令（arm-gcc-none-eabi -O2），以及200bytes左右的RAM占用。mtfmt的其主要特性包括：

* [X] 无除法运算、取模运算的有符号/无符号整数格式化（二进制、八进制、十进制、十六进制）
* [X] 无除法运算、取模运算的有符号/无符号的量化值格式化（q31_t等）
* [ ] ~~无除法运算、取模运算的有符号/无符号定点数格式化~~  **懒**
* [X] 标准日期时间格式化、指定分隔符、项位置的日期和时间格式化
* [X] 字符串格式化
* [X] 指定对齐方式、填充宽度、符号显示方式
* [X] C数组格式化
* [X] O(n)时间复杂度
* [X] 不依赖操作系统，提供用于嵌入式设备的内存分配器
* [X] 提供动态长度字符串，字符串携带 stack allocator
* [X] 提供可传入 `va_list` 函数，变参函数
* [X] 轻松定位parser错误位置以及原因

请注意区分于大多数的格式化器，`MtFmt-Core` **暂不提供**如下的功能：

* 自定义格式化标记
* 自定义错误处理

## 详细描述

这里给出了 `mtfmt` 的格式化语法细节。其使用一个小可爱语言来完成它，姑且称作 `MtFmt-DSL` 。MtFmt 通过 `replacement_field` 区分需要放置的值类型，使用 `{` 和 `}` 包起来。对于 replacement field，其至少需要指定参数位置以及参数类型，且参数位置必须是递增的。若使用 `{ A }` 表示0到多个的A, `[ A ]` 表示可选的A，`{ A }+` 表示1到多个的A，`{ A }?` 表示0或1个A。使用不同的行或者“|”以表示“或”，使用缩进区分符号。下面给出了总体语法。

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
    `%` standard_chrono
    user_def_chrono
```

TODO

## 例子

TODO

## See also

TODO


