<!--
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { createEventDispatcher } from 'svelte'

    /**
     * 项的类型
     */
    type GroupItems = Map<
        string,
        {
            display_name: string
            title?: string
        }
    >

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
                {item[1].display_name}
            </label>
        </div>
    {/each}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .groups {
        width: 100%;
        height: 100%;
        gap: 0.25em;
        display: grid;
        align-items: baseline;
        grid-auto-flow: row dense;
        grid-row-gap: 0.5em;
        grid-column-gap: 0.25em;
        grid-template-rows: 1fr auto;
        grid-template-columns: repeat(auto-fit, minmax(8.5em, 1fr));
    }

    .group-item {
        text-align: center;

        input[type='radio'] {
            display: none;
        }

        label {
            background-color: var(--radio-bg-color);
            display: inline-block;
            width: 100%;
            box-sizing: border-box;
            padding-top: 0.5em;
            padding-bottom: 0.5em;
            white-space: nowrap;
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
