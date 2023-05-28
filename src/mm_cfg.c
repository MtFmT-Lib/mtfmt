// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    mm_cfg.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   打印编译时的环境
 * @version 1.0
 * @date    2023-05-28
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_cfg.h"
#include "mm_typedef.h"

MSTR_EXPORT_API(uint32_t) mstr_configure(void)
{
    uint32_t configure = 0x00;
#if _MSTR_USE_MALLOC
    configure |= MSTRCFG_USE_MALLOC_BIT;
#endif // _MSTR_USE_MALLOC
#if _MSTR_BUILD_DLL
    configure |= MSTRCFG_BUILD_DLL_BIT;
#endif // _MSTR_USE_MALLOC
#if __EMSCRIPTEN__
    configure |= MSTRCFG_USE_WASM_BIT;
#endif // __EMSCRIPTEN__
    return configure;
}
