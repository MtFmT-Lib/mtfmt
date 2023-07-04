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
    import { default_opts, steps } from './install_selector'
    import { generate_new_disable_items } from './install_selector'
    import type { GroupName, SelectedGroup } from './install_selector'

    /**
     * apply函数
     */
    const event_fun = curry_2(apply_svelte)

    /**
     * 不可使用的项
     */
    const disable_items = writable<Map<string, string[]>>(new Map())

    /**
     * 当前选中的值
     */
    const selected = writable<SelectedGroup>(default_opts)

    /**
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as GroupName
        selected.update((last) => {
            const new_val = { ...last, [group]: arg.value }
            // 更新disable items
            const disables = generate_new_disable_items(new_val)
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
