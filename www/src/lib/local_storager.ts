// SPDX-License-Identifier: LGPL-3.0
import { type Optional, Some, None } from '$lib/fp/result'

/**
 * 读取local storager
 */
export function read_local_storager<T = string>(key: string): Optional<T> {
    if (typeof localStorage === 'undefined') {
        return None<T>()
    } else {
        const value = localStorage.getItem(key)
        if (value) {
            return Some<T>(value as T)
        }
        else {
            return None<T>()
        }
    }
}

/**
 * 写入local storager, 如果不存在则不写入
 */
export function write_local_storager<T extends { toString(): string }>(key: string, value: T) {
    if (typeof localStorage !== 'undefined') {
        localStorage.setItem(key, value.toString())
    }
}
