<!--
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { createEventDispatcher } from 'svelte'

    /**
     * 事件类型
     */
    type ButtonEvent = {
        click: void
    }

    const dispatch = createEventDispatcher<ButtonEvent>()

    /**
     * 单击事件
     */
    function on_click() {
        dispatch('click')
    }
</script>

<button class="button" on:click={on_click}>
    <slot />
</button>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .button {
        padding: 0;
        margin: 0;
        padding: 0.6em 0.8em;

        // 边框
        outline: 0;
        border: none;
        border-radius: 2px;

        // 颜色
        color: inherit;
        font-family: $text-font-en;
        font-size: 1.05rem;
        line-height: 1.05rem;
        background-color: var(--button2-bg-color);

        &:hover {
            background-color: var(--button2-hover-bg-color);
        }
    }
</style>
