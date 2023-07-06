// SPDX-License-Identifier: LGPL-3.0

class IntoConvertError extends Error {
    constructor(why: string) {
        super(`Type convert error: ${why}`)
    }
}

/**
 * 转为boolean值
 */
export function into_boolean<T>(v: T): boolean {
    if (typeof (v) === 'string') {
        if (v === 'true') {
            return true
        } else if (v === 'false') {
            return false
        }
        else {
            throw new IntoConvertError('Unsupport value')
        }
    } else if (typeof (v) === 'number') {
        return v !== 0
    } else {
        throw new IntoConvertError('Unsupport value type: ' + typeof (v))
    }
}

/**
 * Record<K, V>转为Map<K, V>
 */
export function record_to_map<K extends string, V, R = V>(
    record: Record<K, V>, map_to: (v: V) => R
): Map<K, R> {
    const vals = Object.entries<V>(record)
    const map = new Map<K, R>()
    for (const val of vals) {
        map.set(val[0] as K, map_to(val[1]))
    }
    return map
}
