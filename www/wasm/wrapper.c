// SPDX-License-Identifier: LGPL-3.0
/**
 * @file    wrapper.c
 * @author  向阳 (hinata.hoshino@foxmail.com)
 * @brief   给online demo准备的wrapper
 * @version 1.0
 * @date    2023-05-17
 *
 * @copyright Copyright (c) 向阳, all rights reserved.
 *
 */
#include "mm_cfg.h"
#include <emscripten.h>
#include <stdint.h>

MSTR_EXPORT_API(uint32_t) mstr_wasm_version()
{
    return 0x01000000;
}
