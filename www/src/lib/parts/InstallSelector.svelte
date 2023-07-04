<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Caption from './Caption.svelte'
    import { curry_2 } from '$lib/fp/fun'
    import { apply_svelte } from '$lib/fp/apply'
    import * as InstallText from '@text/install.md'
    import RadioButtonGroup from '@comp/RadioButtonGroup.svelte'
    import { writable } from 'svelte/store'
    import download_opts from '@text/download.json'

    /**
     * apply函数
     */
    const event_fun = curry_2(apply_svelte)

    /**
     * 分的group name
     */
    type GroupName = 'prebuild' | 'OS'

    /**
     * 每个group的项的type
     */
    type Item = {
        display_name: string
        title?: string
    } & { requires?: string[] }

    /**
     * 步骤的type
     */
    type StepItem = Map<string, Item>

    /**
     * 不可使用的项
     */
    const disable_items = writable<Map<string, string[]>>(new Map())

    /**
     * 当前选中的值
     */
    const selected = writable<Record<GroupName, string | null>>(
        download_opts.default
    )

    /**
     * 步骤
     */
    const steps: Map<GroupName, StepItem> = parsed_from_json()

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
        const opts = download_opts.opts
        const result = new Map<GroupName, StepItem>()
        for (const values of Object.entries(opts)) {
            const item_map = new Map<string, Item>()
            for (const items of Object.entries(values[1])) {
                item_map.set(items[0], items[1])
            }
            result.set(values[0] as GroupName, item_map)
        }
        return result
    }

    /**
     * 从step里面选一个default item, 并排除其在disables中的项
     * 如果找不到会返回null
     */
    function get_default_item(
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
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as GroupName
        selected.update((last) => {
            const new_val = { ...last, [group]: arg.value }
            // 更新disable items
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
            disable_items.set(disables)
            return new_val
        })
    }
</script>

<div>
    <Caption caption_text="Download MtFmt" />
    <div class="install-hints">
        {@html InstallText.html}
    </div>
    <ul class="steps">
        {#each steps as step}
            <li>
                <div class="step-name">{step[0]}</div>
                <div class="step-selector">
                    <RadioButtonGroup
                        group_name={step[0]}
                        items={step[1]}
                        default_selected={$selected[step[0]] ?? undefined}
                        diasble_items={$disable_items.get(step[0]) ?? []}
                        on:change={event_fun(on_change_select)}
                    />
                </div>
            </li>
        {/each}
        <li>
            <div class="finally-step-name">Download</div>
            <div class="step-selector">TODO</div>
        </li>
    </ul>
</div>

<style lang="scss">
    @import '@theme/stylevars';

    .install-hints {
        font-family: $text-font-en;
        letter-spacing: 0.25px;
        line-height: 180%;
    }

    .steps {
        padding: 0;
        margin: 0;
        margin-top: 1.5em;
        margin-bottom: 1em;
        list-style-type: none;

        li {
            display: flex;
            flex-wrap: nowrap;
            flex-direction: row;
            align-items: stretch;
            justify-content: flex-start;
            margin-top: 0.5em;
            margin-bottom: 0.5em;
        }
    }

    .finally-step-name,
    .step-name {
        padding: 0.5em 1.5em;
        flex-basis: 8vw;
        text-transform: capitalize;
    }

    .step-name {
        border-left: 2px solid var(--border-color-middle);
    }

    .step-selector {
        flex-grow: 1;
    }

    .finally-step-name {
        border-left: 2px solid var(--border-color-deep);
    }
</style>
