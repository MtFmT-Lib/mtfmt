// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    example_fmt_int_index.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   整数在不同进制下的格式化的例子
 * @version 1.0
 * @date    2023-07-23
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mtfmt.h"

int main(void)
{
    mstr_print(
        "mstr_print(\"{{0:i32:b}}\\n\", 165); // => {0:i32:b}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:o}}\\n\", 165); // => {0:i32:o}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:d}}\\n\", 165); // => {0:i32:d}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:h}}\\n\", 165); // => {0:i32:h}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:H}}\\n\", 165); // => {0:i32:H}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:x}}\\n\", 165); // => {0:i32:x}\n", 165
    );
    mstr_print(
        "mstr_print(\"{{0:i32:X}}\\n\", 165); // => {0:i32:X}\n", 165
    );
    return 0;
}
