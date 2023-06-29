<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { createEventDispatcher } from 'svelte'

    /**
     * 项的类型
     */
    type ItemType = { value: string; display_name: string }

    /**
     * 事件类型
     */
    type PicknewEvent = {
        picknew: {
            value: string
        }
    }

    /**
     * 项
     */
    export let items: ItemType[] = []

    /**
     * 当前选中的项
     */
    export let selected_item = ''

    /**
     * title
     */
    export let title = 'Picker'

    const dispatch = createEventDispatcher<PicknewEvent>()

    /**
     * 选中项改变
     */
    function select_change(arg: { currentTarget: HTMLSelectElement }) {
        const target = arg.currentTarget
        dispatch('picknew', {
            value: target.value,
        })
    }
</script>

<select {title} bind:value={selected_item} on:change={select_change}>
    {#each items as item}
        <option value={item.value}>{item.display_name}</option>
    {/each}
</select>

<style lang="scss">
    @import '@theme/stylevars.scss';

    select {
        // 不要边框
        outline: 0;
        border: none;

        // 文本对齐
        text-align: center;
        appearance: none;
        -moz-appearance: none;
        -webkit-appearance: none;

        // 边距
        margin: 0;
        padding-top: 0.08em;
        padding-bottom: 0.08em;
        padding-left: 0.6em;
        padding-right: 0.6em;

        // 颜色和背景色
        color: var(--button-color);
        background: none;

        &:active {
            background-color: var(--button-actived-bg-color);
        }

        option {
            background-color: var(--bg-color);
        }

        &:hover,
        &:focus {
            background-color: var(--button-hover-bg-color);
        }
    }
</style>
