<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { writable } from 'svelte/store'
    import generate_toc from '$lib/tocmake'

    /**
     * 内容
     */
    export let contents: Record<
        string,
        {
            html: string
            toc: { level: string; content: string }[]
        }
    >

    /**
     * 语言表
     */
    export let language: string[]

    /**
     * 当前的语言
     */
    let cur_language = writable(language[0])

    /**
     * 设置语言
     */
    function set_language(lang: string) {
        cur_language.set(lang.toLowerCase())
    }
</script>

<div class="markdown">
    <div class="markdown-toolbar">
        <div class="reader-tools">
            <button>BIO</button>
        </div>
        <div class="reader-language">
            {#each language as lang}
                <button on:click={() => set_language(lang)}>
                    <span>{lang.toUpperCase()}</span>
                </button>
            {/each}
        </div>
    </div>
    <div class="markdown-box">
        {@html generate_toc(
            contents[$cur_language].html,
            contents[$cur_language].toc
        )}
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .markdown {
        padding-bottom: 0.5em;
    }

    .markdown-toolbar {
        display: flex;
        justify-content: space-between;
        align-items: baseline;
        flex-direction: row;
        color: mix($bg-color, $text-color, 40%);

        // 颜色和padding
        padding-top: 0.25em;
        padding-bottom: 0.5em;

        // 边框
        border-top: 1px solid $border-color;
    }

    .reader-tools button,
    .reader-language button {
        outline: 0;
        border: none;
        color: $hpyer-link-color;
        background-color: $bg-color;
        padding: 0.25em 0.25em;

        &:hover {
            background-color: mix($text-color, $bg-color, 15%);
        }

        &:active {
            background-color: mix($text-color, $bg-color, 40%);
        }
    }

    .markdown-box {
        line-height: 150%;
        font-size: 1.1rem;
    }
</style>
