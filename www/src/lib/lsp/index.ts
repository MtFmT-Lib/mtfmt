// SPDX-License-Identifier: LGPL-3.0

/**
 * 词法高亮
 */
export const token_provider = (async () => {
    const module = await import('./langtoken')
    return module.default
})()
