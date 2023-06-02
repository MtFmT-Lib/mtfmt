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
