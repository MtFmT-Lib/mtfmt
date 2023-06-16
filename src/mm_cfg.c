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
#if _MSTR_USE_HARDWARE_DIV
    configure |= MSTRCFG_BUILD_HARDWARE_DIV;
#endif // _MSTR_USE_HARDWARE_DIV
#if _MSTR_USE_STD_IO
    configure |= MSTRCFG_USE_STD_IO;
#endif // _MSTR_USE_STD_IO
    // 使用的编译器信息
    configure |= MSTR_BUILD_CC << 12;
    // ret
    return configure;
}
