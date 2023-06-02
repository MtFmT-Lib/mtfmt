<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->

<script lang="ts">
    import { onMount } from 'svelte'
    import { curry_2 } from '$lib/fp/fun'
    import theme_info from './theme_storager'
    import { register_window_event } from './theme_logic'
    import { change_theme, set_default_theme } from './theme_logic'

    /**
     * 需要用到window.XXX的东东
     */
    function on_window(window: Window) {
        register_window_event(window)
    }

    /**
     * 页面完成更新后倒腾这个
     */
    onMount(() => {
        set_default_theme(document, $theme_info)
        theme_info.subscribe(curry_2(change_theme)(document))
    })
</script>

<svelte:window use:on_window />

<div class="app-content">
    <slot />
</div>

<style lang="scss">
    .app-content {
        width: 100%;
        height: 100%;
    }
</style>
