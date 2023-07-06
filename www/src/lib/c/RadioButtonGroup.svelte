<!--
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { createEventDispatcher } from 'svelte'
    import type { GroupItems } from './radio_button_group'

    /**
     * 事件类型
     */
    type RadioGroupEvent = {
        change: {
            group: string
            value: string
        }
    }

    /**
     * 项列表
     */
    export let items: GroupItems = new Map()

    /**
     * 控制哪些项不可选
     */
    export let diasble_items: string[] = []

    /**
     * 组名
     */
    export let group_name = 'radiogroup'

    /**
     * 默认选中项
     */
    export let default_selected: string | undefined = undefined

    const dispatch = createEventDispatcher<RadioGroupEvent>()

    /**
     * 单击选中它们的事件
     */
    function on_click(key: string) {
        dispatch('change', {
            value: key,
            group: group_name,
        })
    }
</script>

<div class="groups">
    {#each items as item}
        <div class="group-item">
            {#if diasble_items.includes(item[0])}
                <input
                    type="radio"
                    name={group_name}
                    value={item[0]}
                    id={item[0]}
                    class="radio-disable"
                    title={item[1].title ?? item[1].display_name}
                    on:click={() => on_click(item[0])}
                    disabled
                    checked={false}
                />
            {:else}
                <input
                    type="radio"
                    name={group_name}
                    value={item[0]}
                    id={item[0]}
                    class="radio"
                    title={item[1].title ?? item[1].display_name}
                    on:click={() => on_click(item[0])}
                    checked={default_selected
                        ? default_selected === item[0]
                        : false}
                />
            {/if}
            <label for={item[0]} title={item[1].title ?? item[1].display_name}>
                <div>{item[1].display_name}</div>
            </label>
        </div>
    {/each}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .groups {
        width: inherit;
        height: inherit;
        gap: 0.25em;
        display: grid;
        align-items: baseline;
        grid-auto-flow: row dense;
        grid-row-gap: 0.5em;
        grid-column-gap: 0.25em;
        grid-template-rows: 1fr auto;
        grid-template-columns: repeat(auto-fit, minmax(8.5em, 1fr));

        @media screen and (width < $sm) {
            align-items: stretch;
            grid-row-gap: 0.25em;
            grid-column-gap: 0.2em;
            grid-template-columns: repeat(auto-fit, minmax(6.5em, 1fr));
        }
    }

    .group-item {
        text-align: center;

        input[type='radio'] {
            display: none;
        }

        label {
            // 宽高
            width: 100%;
            height: 100%;

            // 颜色和断行
            background-color: var(--radio-bg-color);
            box-sizing: border-box;
            white-space: nowrap;

            // 内容垂直居中
            display: flex;
            align-items: center;
            justify-content: center;

            // 内边距
            padding-top: 0.5em;
            padding-bottom: 0.5em;

            @media screen and (width < $sm) {
                white-space: break-spaces;
            }
        }

        input[type='radio']:disabled + label {
            color: var(--radio-disable-color);
            background-color: var(--radio-disable-bg-color);
        }

        input[type='radio'].radio:checked + label {
            background-color: var(--radio-selected-bg-color);
        }

        input[type='radio'].radio:hover + label {
            background-color: var(--radio-hover-bg-color);
        }
    }
</style>
