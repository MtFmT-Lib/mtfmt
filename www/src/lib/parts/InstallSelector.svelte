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
    }

    /**
     * 步骤的type
     */
    type StepItem = Map<string, Item & { requires?: string[] }>

    /**
     * 依赖关系
     */
    const deps = writable<Record<GroupName, string[]>>()

    /**
     * 当前选中的值
     */
    const selected = writable<Record<GroupName, string>>()

    /**
     * 步骤
     */
    const steps: Record<GroupName, StepItem> = {
        prebuild: new Map([
            [
                'dylib',
                {
                    display_name: 'Dymatic library',
                    title: 'Dymatic library prebuild',
                },
            ],
            [
                'lib',
                {
                    display_name: 'Static library',
                    title: 'Static library prebuild',
                },
            ],
        ]),
        OS: new Map([
            [
                'win',
                {
                    display_name: 'Windows',
                    requires: ['lib', 'dylib'],
                },
            ],
            [
                'unix',
                {
                    display_name: 'Linux',
                },
            ],
        ]),
    }

    selected.subscribe((new_val) => {
        if (new_val === undefined) {
            return
        }
        console.log(new_val)
    })

    /**
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as GroupName
        selected.update((last) => ({ ...last, [group]: arg.value }))
    }

    /**
     * 取得group name
     */
    function disable_items(
        items: Record<GroupName, string[]> | undefined,
        name: string
    ): string[] {
        return items ? items[name as GroupName] : []
    }
</script>

<div>
    <Caption caption_text="Download MtFmt" />
    <div class="install-hints">
        {@html InstallText.html}
    </div>
    <ul class="steps">
        {#each Object.entries(steps) as step}
            <li>
                <div class="step-name">{step[0]}</div>
                <div class="step-selector">
                    <RadioButtonGroup
                        group_name={step[0]}
                        items={step[1]}
                        diasble_items={disable_items($deps, step[0])}
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
