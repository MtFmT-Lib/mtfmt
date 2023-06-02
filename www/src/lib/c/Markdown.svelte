<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { writable } from 'svelte/store'
    import generate_toc from './markdown_toc'
    import theme_info, { type Theme, set_theme } from './theme_storager'

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

    /**
     * 设置主题
     */
    function set_current_theme(theme: Theme) {
        set_theme(theme)
    }
</script>

<div class="markdown">
    <div class="markdown-toolbar">
        <div class="reader-tools">
            <!-- bio 阅读 -->
            {#if $cur_language === 'en'}
                <button>BIO</button>
            {/if}
            <!-- 主题 -->
            {#each $theme_info.themes as t}
                {#if $theme_info.cur_theme === t}
                    <button id="actived-button">
                        {t.toUpperCase()}
                    </button>
                {:else}
                    <button on:click={() => set_current_theme(t)}>
                        {t.toUpperCase()}
                    </button>
                {/if}
            {/each}
        </div>
        <div class="reader-language">
            {#each language as lang}
                <button on:click={() => set_language(lang)}>
                    {lang.toUpperCase()}
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

        // 边框
        border: none;
        border-left: 2px solid $border-color;

        // 颜色和背景色
        color: $hpyer-link-color;
        background-color: $bg-color;

        // 边距
        margin: 0;
        padding-top: 0.1em;
        padding-bottom: 0.1em;
        padding-left: 0.5em;
        padding-right: 0.5em;

        &:hover {
            background-color: mix($text-color, $bg-color, 15%);
        }

        &:active {
            background-color: mix($text-color, $bg-color, 40%);
        }

        &:first-child {
            border: none;
        }
    }

    .reader-tools button[id='actived-button'] {
        color: mix($hpyer-link-color, black, 40%);
    }

    .markdown-box {
        line-height: 150%;
        font-size: 1.1rem;
    }
</style>
