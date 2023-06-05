// SPDX-License-Identifier: LGPL-3.0

/**
 * 柯里化 (2参数)
 */
export function curry_2<A1, A2, R>(f: (a: A1, b: A2) => R): (a: A1) => ((b: A2) => R) {
    return (a: A1) => (b: A2) => f(a, b)
}
