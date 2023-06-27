// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_fmt_chrono.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   日期时间测试
 * @version 1.0
 * @date    2023-06-03
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "helper.h"
#include "main.h"
#include "mtfmt.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

// 默认的日期时间
static const MStrTime time_value = {
    .year = 0x2023,
    .month = 0x05,
    .day = 0x06,
    .hour = 0x16,
    .minute = 0x55,
    .second = 0x19,
    .week = 0x6,
    .sub_second = 0x1234,
};

void fmt_chrono_default_f(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:t:%f}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@2023-05-06 16:55:19.1234 6@");
    mstr_free(&s);
}

void fmt_chrono_default_g(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:t:%g}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@2023-05-06 16:55:19.1234@");
    mstr_free(&s);
}

void fmt_chrono_userdef_year(void)
{
    // TODO
}

void fmt_chrono_userdef_month(void)
{
    const MStrTime time_value_sing = {.month = 0x01};
    const MStrTime time_value_doub = {.month = 0x10};
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1位~2位
    EVAL(mstr_format(
        &s, "@{0:t:%M}@{1:t:%M}@", 2, &time_value_sing, &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@1@10@");
    // 固定2位
    mstr_clear(&s);
    EVAL(mstr_format(
        &s,
        "@{0:t:%MM}@{1:t:%MM}@",
        2,
        &time_value_sing,
        &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@01@10@");
    mstr_free(&s);
}

void fmt_chrono_userdef_day(void)
{
    const MStrTime time_value_sing = {.day = 0x01};
    const MStrTime time_value_doub = {.day = 0x10};
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1位~2位
    EVAL(mstr_format(
        &s, "@{0:t:%d}@{1:t:%d}@", 2, &time_value_sing, &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@1@10@");
    // 固定2位
    mstr_clear(&s);
    EVAL(mstr_format(
        &s,
        "@{0:t:%dd}@{1:t:%dd}@",
        2,
        &time_value_sing,
        &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@01@10@");
    mstr_free(&s);
}

void fmt_chrono_userdef_hour(void)
{
    const MStrTime time_value_sing = {.hour = 0x01};
    const MStrTime time_value_doub = {.hour = 0x10};
    MString s;
    EVAL(mstr_create_empty(&s));
    // 24h: 1位~2位
    EVAL(mstr_format(
        &s, "@{0:t:%H}@{1:t:%H}@", 2, &time_value_sing, &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@1@10@");
    // 24h: 固定2位
    mstr_clear(&s);
    EVAL(mstr_format(
        &s,
        "@{0:t:%HH}@{1:t:%HH}@",
        2,
        &time_value_sing,
        &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@01@10@");
    mstr_free(&s);
}

void fmt_chrono_userdef_minute(void)
{
    const MStrTime time_value_sing = {.minute = 0x01};
    const MStrTime time_value_doub = {.minute = 0x10};
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1位~2位
    EVAL(mstr_format(
        &s, "@{0:t:%m}@{1:t:%m}@", 2, &time_value_sing, &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@1@10@");
    // 固定2位
    mstr_clear(&s);
    EVAL(mstr_format(
        &s,
        "@{0:t:%mm}@{1:t:%mm}@",
        2,
        &time_value_sing,
        &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@01@10@");
    mstr_free(&s);
}

void fmt_chrono_userdef_second(void)
{
    const MStrTime time_value_sing = {.second = 0x01};
    const MStrTime time_value_doub = {.second = 0x10};
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1位~2位
    EVAL(mstr_format(
        &s, "@{0:t:%s}@{1:t:%s}@", 2, &time_value_sing, &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@1@10@");
    // 固定2位
    mstr_clear(&s);
    EVAL(mstr_format(
        &s,
        "@{0:t:%ss}@{1:t:%ss}@",
        2,
        &time_value_sing,
        &time_value_doub
    ));
    ASSERT_EQUAL_STRING(&s, "@01@10@");
    mstr_free(&s);
}

void fmt_chrono_userdef_subsecond(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    // 1位
    EVAL(mstr_format(&s, "@{0:t:%x}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@1@");
    // 前2位
    mstr_clear(&s);
    EVAL(mstr_format(&s, "@{0:t:%xx}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@12@");
    // 前3位
    mstr_clear(&s);
    EVAL(mstr_format(&s, "@{0:t:%xxx}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@123@");
    // 前4位
    mstr_clear(&s);
    EVAL(mstr_format(&s, "@{0:t:%xxxx}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@1234@");
    mstr_free(&s);
}

void fmt_chrono_userdef_week(void)
{
    MString s;
    EVAL(mstr_create_empty(&s));
    EVAL(mstr_format(&s, "@{0:t:%w}@", 1, &time_value));
    ASSERT_EQUAL_STRING(&s, "@6@");
    mstr_free(&s);
}
