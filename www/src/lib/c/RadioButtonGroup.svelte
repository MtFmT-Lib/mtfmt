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
        display: flex;
        flex-wrap: wrap;
        flex-direction: row;
        align-items: center;
        justify-content: flex-start;
    }

    .group-item {
        text-align: center;
        flex: 1 1 0;

        input[type='radio'] {
            display: none;
        }

        label {
            background-color: #ccc;
            display: inline-block;
            width: 100%;
            box-sizing: border-box;
            padding-top: 0.5em;
            padding-bottom: 0.5em;
            white-space: nowrap;
        }

        input[type='radio']:disabled + label {
            background-color: #f3f3f3;
        }

        input[type='radio'].radio:checked + label {
            background-color: #aaa;
        }

        input[type='radio'].radio:hover + label {
            background-color: #aaa;
        }
    }
</style>
