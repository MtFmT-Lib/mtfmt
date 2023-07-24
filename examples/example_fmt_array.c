// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    example_fmt_array.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   数组格式化的例子
 * @version 1.0
 * @date    2023-07-24
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.h"

int main(void)
{
    const uint32_t array[5] = {0x12, 0x34, 0x56, 0x78, 0x90};
    const usize_t array_size = 5;
    mstr_print(
        "mstr_print(\"{{[0:i32]}}\\n\", array, array_size);   // => "
        "{[0:i32]}\n",
        array,
        array_size
    );
    mstr_print(
        "mstr_print(\"{{[0:i32:x]}}\\n\", array, array_size); // => "
        "{[0:i32:x]}\n",
        array,
        array_size
    );
    return 0;
}
