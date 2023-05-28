// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    string.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   字符串测试
 * @version 1.0
 * @date    2023-05-28
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_heap.h"
#include "unity.h"
#include <stdio.h>

void setUp(void)
{
#if !_MSTR_USE_MALLOC
    mheap_init();
#endif
}

void tearDown(void)
{
}

int main()
{

    return 0;
}
