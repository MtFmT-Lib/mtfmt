<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->

<script lang="ts">
    import { onMount, onDestroy } from 'svelte'
    import theme_info from './theme_storager'
    import * as MonacoWrap from './monaco_wrap'
    import * as LSP from '$lib/lsp'

    /**
     * 编辑器的div
     */
    let div_element: HTMLDivElement

    /**
     * Workers, 默认是空的
     */
    export let workers: MonacoWrap.EditorWorkerMap = new Map()

    /**
     * 默认显示的代码的语言
     */
    export let default_lang = 'cpp'

    /**
     * 默认显示的代码
     */
    export let default_code = ''

    onMount(async () =>
        MonacoWrap.load_editor_module(
            div_element,
            workers,
            $theme_info,
            default_lang,
            default_code,
            [
                {
                    language: { id: 'cpp' },
                    token_provider: LSP.token_provider,
                },
            ]
        )
    )

    onDestroy(MonacoWrap.destroy_instance)

    /**
     * 注册和window相关的event
     */
    function on_window(window: Window) {
        window.addEventListener('resize', MonacoWrap.re_layout, false)
    }
</script>

<svelte:window use:on_window />

<div class="code-editor" bind:this={div_element} />

<style lang="scss">
    .code-editor {
        width: 100%;
        height: 24vh;
    }
</style>
