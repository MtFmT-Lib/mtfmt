// SPDX-License-Identifier: LGPL-3.0
/**
 * 项细节
 */
export type ItemDetails = {
    type: string
    value: string
    text?: string
}

/**
 * 项内容
 */
export type ItemContent = {
    brief: string
    details: ItemDetails
}

/**
 * 项类型
 */
export type Items = Map<string, ItemContent>
