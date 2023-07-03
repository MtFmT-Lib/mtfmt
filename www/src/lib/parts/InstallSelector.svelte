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
    const selected = writable<Record<GroupName, string>>(download_opts.default)

    /**
     * 步骤
     */
    const steps: Map<GroupName, StepItem> = parsed_from_json()

    selected.subscribe((new_val) => {
        if (new_val === undefined) {
            return
        }
        const result: Map<GroupName, string[]> = new Map()
        for (const key of steps.keys()) {
            // 如果有 requires, 确认其在 selected_values 中, 不然认为不可选中
            const disables: string[] = []
            steps.get(key)?.forEach((value, key) => {
                if (dete_should_disable(new_val, value)) {
                    disables.push(key)
                }
            })
            if (disables.length > 0) {
                result.set(key, disables)
            }
        }
        disable_items.set(result)
    })

    /**
     * 判断item是否应该被disable
     */
    function dete_should_disable(
        cur_selected: Record<GroupName, string>,
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
            const filted = req_item.filter((s) => values.includes(s))
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
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as GroupName
        selected.update((last) => ({ ...last, [group]: arg.value }))
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
                        default_selected={$selected[step[0]]}
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
