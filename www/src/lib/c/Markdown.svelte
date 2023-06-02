<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { writable } from 'svelte/store'
    import generate_toc from './markdown_toc'
    import theme_info from './theme_storager'
    import { type Theme, set_theme, DEFAULT_THEME } from './theme_storager'

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
     * 当前的主题名
     */
    let cur_theme: Theme | 'default' = DEFAULT_THEME

    /**
     * 是否开启bio-reader
     */
    let enable_bio_reader = writable(false)

    /**
     * 设置语言
     */
    function set_language(lang: string) {
        cur_language.set(lang.toLowerCase())
    }

    /**
     * 切换bio reading
     */
    function toggle_bio_mode() {
        enable_bio_reader.set(!$enable_bio_reader)
    }

    /**
     * 更新选中的主题
     */
    function update_cur_theme() {
        if (cur_theme === 'default') {
            set_theme()
        } else {
            set_theme(cur_theme)
        }
    }
</script>

<div class="markdown">
    <div class="markdown-toolbar">
        <div class="reader-tools">
            <!-- bio 阅读 -->
            {#if $cur_language === 'en'}
                {#if $enable_bio_reader}
                    <button id="actived-button" on:click={toggle_bio_mode}>
                        <span>B</span>
                    </button>
                {:else}
                    <button on:click={toggle_bio_mode}>
                        <span>B</span>
                    </button>
                {/if}
            {/if}
            <!-- 主题 -->
            <select bind:value={cur_theme} on:change={update_cur_theme}>
                {#each $theme_info.themes as t}
                    <option value={t}>
                        {t.toUpperCase()}
                    </option>
                {/each}
                <!-- system -->
                <option value="default">SYS</option>
            </select>
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
        border-top: 1px solid var(--border-color);
    }

    .reader-tools button,
    .reader-tools select,
    .reader-language button {
        outline: 0;

        // 边框
        border: none;
        border-left: 2px solid var(--border-color);

        // 颜色和背景色
        color: var(--button-color);
        background: none;

        // 边距
        margin: 0;
        padding-top: 0.1em;
        padding-bottom: 0.1em;
        padding-left: 0.5em;
        padding-right: 0.5em;

        &:first-child {
            border: none;
        }
    }

    .reader-tools button,
    .reader-language button {
        &:hover {
            background-color: var(--button-hover-bg-color);
        }

        &:active {
            background-color: var(--button-actived-bg-color);
        }
    }

    .reader-tools select {
        text-align: center;
        appearance: none;
        -moz-appearance: none;
        -webkit-appearance: none;

        option {
            background-color: var(--bg-color);
        }

        &:hover,
        &:focus {
            background-color: var(--button-hover-bg-color);
        }
    }

    .reader-tools button[id='actived-button'] {
        background-color: var(--button-actived-bg-color);
    }

    .markdown-box {
        line-height: 150%;
        font-size: 1.1rem;
    }
</style>
