// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    example_fmt_align.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   不同对齐方式下的格式化的例子
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
        "mstr_print(\"|{{0:i32:=8}}|\\n\", 1234); // => |{0:i32:=8}|\n",
        1234
    );
    mstr_print(
        "mstr_print(\"|{{0:i32:<8}}|\\n\", 1234); // => |{0:i32:<8}|\n",
        1234
    );
    mstr_print(
        "mstr_print(\"|{{0:i32:>8}}|\\n\", 1234); // => |{0:i32:>8}|\n",
        1234
    );
    return 0;
}
