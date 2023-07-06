// SPDX-License-Identifier: LGPL-3.0

/**
 * 项
 */
export type GroupItem = {
    display_name: string
    title?: string
}

/**
 * 项的类型
 */
export type GroupItems = Map<string, GroupItem>
