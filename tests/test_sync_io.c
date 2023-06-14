// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_sync_io.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   同步IO的测试
 * @version 1.0
 * @date    2023-06-14
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mtfmt.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static MStrIOCallback cb;

static mstr_result_t leak_write(
    void* ctx, const byte_t* data, usize_t len
)
{
    memcpy(ctx, data, len);
    return MStr_Ok;
}

void sync_io_write(void)
{
    char buff[256];
    EVAL(mstr_io_init(buff, &cb, leak_write));
    EVAL(mstr_ioformat(&cb, "{0:s}", 1, "Test"));
    TEST_ASSERT_TRUE(memcmp(buff, "Test", 4) == 0);
}
