// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    test_cimpl_parser.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   parser的测试 (主要是各个err ...)
 * @version 1.0
 * @date    2023-07-16
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_parser.h"
#include "test_helper.h"
#include "test_main.h"
#include "unity.h"
#include <stddef.h>
#include <stdio.h>

static mstr_result_t do_parse(const char*, MStrFmtParseResult*);

void parser_err_invaild_begin(void)
{
    mstr_result_t res;
    MStrFmtParseResult out;
    // 不可作为起始的字符
    res = do_parse("s", &out);
    ASSERT_EQUAL_VALUE(res, MStr_Err_MissingReplacement);
}

void parser_err_invaild_end(void)
{
    mstr_result_t res;
    MStrFmtParseResult out;
    // 缺失 "}"
    res = do_parse("{0:i32", &out);
    ASSERT_EQUAL_VALUE(res, MStr_Err_MissingRightBrace);
}

void parser_err_invaild_fmt_spec(void)
{
    mstr_result_t res;
    MStrFmtParseResult out;
    // 不被识别的fmt spec
    res = do_parse("{0:i32:v}", &out);
    ASSERT_EQUAL_VALUE(res, MStr_Err_UnsupportFormatType);
}

// 恼 也许我们应该考虑换个方式测测...
// 这样子好笨蛋

/**
 * @brief 调用parser解析输入
 *
 */
static mstr_result_t do_parse(const char* inp, MStrFmtParseResult* out)
{
    mstr_result_t result;
    MStrFmtParserState* state;
    byte_t state_memory[MFMT_PARSER_STATE_SIZE];
    mstr_fmt_parser_init(state_memory, inp, &state);
    result = mstr_fmt_parse_goal(state, out);
    return result;
}
