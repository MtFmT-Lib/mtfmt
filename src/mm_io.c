// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_io.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   IO
 * @version 1.0
 * @date    2023-06-07
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_io.h"
#include "mm_cfg.h"
#include "mm_fmt.h"
#include "mm_string.h"

//
// private:
//

static mstr_result_t stdio_callback(void*, const byte_t*, usize_t);

/**
 * @brief stdout IO
 *
 */
static MStrIOCallback mstr_stdout = {
    .capture = NULL,
    .io_write = stdio_callback,
};

//
// public:
//

MSTR_EXPORT_API(mstr_result_t)
mstr_io_init(void* context, MStrIOCallback* obj, MStrIOWrite cb_write)
{
    obj->capture = context;
    obj->io_write = cb_write;
    return MStr_Ok;
}

MSTR_EXPORT_API(MStrIOCallback*) mstr_get_stdout(void)
{
    return &mstr_stdout;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_ioformat(
    MStrIOCallback* io, const char* fmt, usize_t fmt_place, ...
)
{
    va_list ap;
    mstr_result_t res;
    va_start(ap, fmt_place);
    res = mstr_iovformat(io, fmt, fmt_place, &ap);
    va_end(ap);
    return res;
}

MSTR_EXPORT_API(mstr_result_t)
mstr_iovformat(
    MStrIOCallback* io,
    const char* fmt,
    usize_t fmt_place,
    va_list* ap_ptr
)
{
    MString buff;
    mstr_result_t res_create;
    mstr_result_t res = MStr_Ok;
    MSTR_AND_THEN(res, mstr_create_empty(&buff));
    res_create = res;
    // 进行格式化
    MSTR_AND_THEN(res, mstr_vformat(fmt, &buff, fmt_place, ap_ptr));
    // 写到输出
    MSTR_AND_THEN(
        res,
        io->io_write(io->capture, (const byte_t*)buff.buff, buff.length)
    );
    // 释放
    if (MSTR_SUCC(res_create)) {
        mstr_free(&buff);
    }
    return res;
}

static mstr_result_t stdio_callback(
    void* ctx, const byte_t* data, usize_t len
)
{
#if _MSTR_USE_STD_IO
    usize_t nu = sizeof(usize_t);
    usize_t ncnt = len / nu;
    usize_t nrem = len % nu;
    fwrite(data, nu, ncnt, stdout);
    fwrite(data + ncnt * nu, 1, nrem, stdout);
    return MStr_Ok;
#endif
    (void)ctx;
    (void)data;
    (void)len;
    return MStr_Err_NoImplemention;
}
