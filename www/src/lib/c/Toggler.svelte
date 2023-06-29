<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { writable } from 'svelte/store'
    import { createEventDispatcher } from 'svelte'

    /**
     * 是否选中
     */
    export let checked = false

    /**
     * title
     */
    export let title = 'Toggle button'

    /**
     * 当前state
     */
    const cur_state = writable(checked)

    /**
     * 事件类型
     */
    type TogglerEvent = {
        toggle: {
            state: boolean
        }
    }

    const dispatch = createEventDispatcher<TogglerEvent>()

    /**
     * state改变
     */
    function toggle(new_state: boolean) {
        cur_state.set(new_state)
        dispatch('toggle', {
            state: new_state,
        })
    }
</script>

{#if $cur_state}
    <button {title} id="actived-button" on:click={() => toggle(false)}>
        <slot />
    </button>
{:else}
    <button {title} on:click={() => toggle(true)}>
        <slot />
    </button>
{/if}

<style lang="scss">
    @import '@theme/stylevars.scss';

    button {
        outline: 0;
        border: none;
        display: block;

        // 边距
        margin: 0;
        padding-top: 0.08em;
        padding-bottom: 0.08em;
        padding-left: 0.6em;
        padding-right: 0.6em;

        // 颜色和背景色
        color: var(--button-color);
        background: none;

        &:hover {
            background-color: var(--button-hover-bg-color);
        }

        &:active {
            background-color: var(--button-actived-bg-color);
        }
    }

    button[id='actived-button'] {
        background-color: var(--button-actived-bg-color);
    }
</style>
