// SPDX-License-Identifier: LGPL-3.0
import download_opts from '@text/download.json'
import { record_to_map } from '$lib/fp/cast'
import type { GroupItem } from '@comp/radio_button_group'

/**
 * 分的group name
 */
export type GroupName = string

/**
 * 选中的Group类型
 */
export type SelectedGroup = Record<GroupName, string | null>

/**
 * 每个group的项的type
 */
export type Item = GroupItem & { requires?: string[] }

/**
 * 步骤的type
 */
export type StepItem = Map<string, Item>

/**
 * 步骤
 */
export const steps: Map<GroupName, StepItem> = parsed_from_json()

/**
 * 下载选项
 */
export const default_opts = download_opts.default

/**
 * 从step里面选一个default item, 并排除其在disables中的项
 * 如果找不到会返回null
 */
export function get_default_item(
    key: GroupName,
    disables: Map<GroupName, string[]>
): string | null {
    const disable_items = disables.get(key) ?? []
    for (const item_group of steps.get(key) ?? []) {
        const fn = (s: string) => s === item_group[0]
        if (disable_items.findIndex(fn) === -1) {
            // 选择该项作为合理的项
            return item_group[0]
        }
    }
    return null
}

/**
 * 按照 new_val 生成新的disable项
 * 
 */
export function generate_new_disable_items(
    new_val: SelectedGroup
): Map<GroupName, string[]> {
    let disables = new Map<GroupName, string[]>()
    // 保证当前select不是disable items
    for (const key of steps.keys()) {
        disables = dete_disable_items(new_val)
        const disable_items = disables.get(key) ?? []
        const in_newval = (s: string) => s === new_val[key] ?? false
        if (disable_items.findIndex(in_newval) !== -1) {
            // 尝试选择一个合理的项
            new_val[key] = get_default_item(key, disables)
        }
    }
    return disables
}

/**
 * 取得下载配置
 */
export function get_download_configure(
    cur_selected: Record<GroupName, string | null>
): Record<'addr' | 'type', string> | null {
    const downloads = download_opts.download
    const values = Object.values(cur_selected)
        .filter(s => s !== null)
        .map(s => s as string)
    // 找到每个项在当前选中项中的匹配个数
    const download_matchs = downloads.flatMap((s, org_idx) => {
        const conds = s.cond.split('&')
        const filter_res = conds.filter(v => {
            const idx = values.findIndex(f => f === v)
            return idx !== -1
        })
        return { origin_index: org_idx, count: filter_res.length }
    })
    // 找到最佳匹配(即count最大的那个)
    const best_matched = download_matchs.reduce(
        (pre_val, cur_val) => pre_val.count > cur_val.count ? pre_val : cur_val,
        download_matchs[0]
    )
    return best_matched.count > 0 ? downloads[best_matched.origin_index] : null
}

/**
 * 计算出应该被disable的项
 */
function dete_disable_items(
    cur_selected: Record<GroupName, string | null>
): Map<GroupName, string[]> {
    const result = new Map<GroupName, string[]>()
    for (const key of steps.keys()) {
        const disables: string[] = []
        steps.get(key)?.forEach((value, key) => {
            if (dete_should_disable(cur_selected, value)) {
                disables.push(key)
            }
        })
        // 记录下来
        result.set(key, disables)
    }
    return result
}

/**
 * 判断item是否应该被disable
 */
function dete_should_disable(
    cur_selected: Record<GroupName, string | null>,
    item: Item
): boolean {
    if (item.requires === undefined) {
        return false
    }
    const requires = item.requires
    const values = Object.values(cur_selected)
    // 每一条requires是or的关系
    for (const require of requires) {
        // 分开每个and的依赖项
        const req_item = require.split('&')
        const filted = req_item.filter((s) =>
            values ? values.includes(s) : false
        )
        if (filted.length === req_item.length) {
            // 存在requires满足的情况
            return false
        }
    }
    return true
}

/**
 * 从json生成选中项
 */
function parsed_from_json(): Map<GroupName, StepItem> {
    return record_to_map(
        download_opts.opts,
        s => record_to_map<string, Item>(s, r => r)
    )
}
