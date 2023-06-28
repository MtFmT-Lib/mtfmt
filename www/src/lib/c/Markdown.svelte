<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { writable } from 'svelte/store'
    import { into_boolean } from '$lib/fp/cast'
    import * as Storager from '$lib/local_storager'
    import { set_language_attrs } from './theme_lang'
    import { get_build_in_html } from './markdown_trans'
    import theme_info, { type Theme } from './theme_storager'
    import { set_theme, get_storager_theme } from './theme_storager'
    import type { BuildinContent, LanguageKey } from './markdown_trans'
    import get_content_html, { get_support_languages } from './markdown_trans'

    /**
     * 文本颜色
     */
    type TextColor = 'inherit' | 'var(--weak-color)'

    /**
     * 内容
     */
    export let contents: BuildinContent

    /**
     * 支持的语言
     */
    let support_languages = get_support_languages(contents)

    /**
     * 语言配置项
     */
    const LANGUAGE_ITEM_KEY = 'language'

    /**
     * bio-reading模式配置项
     */
    const BIO_READING_ITEM_KEY = 'bio-reading'

    /**
     * html内容
     */
    let html_content = writable<string>(get_build_in_html('en', contents))

    /**
     * 文本颜色
     */
    let text_color = writable<TextColor>('inherit')

    /**
     * 是否开启bio-reader
     */
    let enable_bio_reader = writable(
        Storager.read_local_storager(BIO_READING_ITEM_KEY)
            .map(into_boolean)
            .or(false)
    )

    enable_bio_reader.subscribe((enable) => {
        if (enable) {
            text_color.set('var(--weak-color)')
        } else {
            text_color.set('inherit')
        }
    })

    /**
     * 当前的语言
     */
    let cur_language = writable(get_default_language())

    cur_language.subscribe((language) => {
        // 设置语言attrs
        set_language_attrs(language)
        // 更新bio-reading模式
        if ($enable_bio_reader && language !== 'en') {
            // 仅在英文下允许使用bio reading
            enable_bio_reader.set(false)
        } else {
            const las = Storager.read_local_storager(BIO_READING_ITEM_KEY)
                .map(into_boolean)
                .or(false)
            // 恢复上次的选项
            enable_bio_reader.set(las)
        }
        // 加载内容
        get_content_html(html_content, language, contents)
    })

    /**
     * 切换bio reading
     */
    function set_bio_mode(enable: boolean) {
        enable_bio_reader.set(enable)
        Storager.write_local_storager(BIO_READING_ITEM_KEY, enable)
    }

    /**
     * 更新选中的主题
     */
    function update_cur_theme(arg: { currentTarget: HTMLSelectElement }) {
        const target = arg.currentTarget
        const cur_theme = target.value as Theme
        set_theme(cur_theme)
    }

    /**
     * 更新选中的语言
     */
    function update_cur_language(arg: { currentTarget: HTMLSelectElement }) {
        const target = arg.currentTarget
        const language = target.value as LanguageKey
        cur_language.set(language)
        Storager.write_local_storager(LANGUAGE_ITEM_KEY, language)
    }

    /**
     * 取得默认的语言
     */
    function get_default_language(): LanguageKey {
        return Storager.read_local_storager(LANGUAGE_ITEM_KEY)
            .map((s) => s as LanguageKey)
            .or_map(() => {
                let language: LanguageKey
                if (typeof navigator === 'undefined') {
                    language = 'en'
                } else {
                    const lut = new Map<string, LanguageKey>([
                        ['en', 'en'],
                        ['en-US', 'en'],
                        ['zh-CN', 'zh'],
                    ])
                    language = lut.get(navigator.language) ?? 'en'
                }
                Storager.write_local_storager(LANGUAGE_ITEM_KEY, language)
                return language
            })
    }
</script>

<div class="markdown">
    <div class="markdown-toolbar">
        <div class="reader-tools">
            <!-- bio 阅读 -->
            {#if $cur_language === 'en'}
                {#if $enable_bio_reader}
                    <button
                        id="actived-button"
                        title="Disable the bio-reading mode"
                        on:click={() => set_bio_mode(!$enable_bio_reader)}
                    >
                        <span>B</span>
                    </button>
                {:else}
                    <button
                        title="Active the bio-reading mode"
                        on:click={() => set_bio_mode(!$enable_bio_reader)}
                    >
                        <span>B</span>
                    </button>
                {/if}
            {/if}
        </div>
        <div class="reader-language">
            <!-- 主题 -->
            <select
                title="Choice theme"
                value={get_storager_theme()}
                on:change={update_cur_theme}
            >
                {#each $theme_info.themes as t}
                    <option value={t}>
                        {t.toUpperCase()}
                    </option>
                {/each}
            </select>
            <!-- 语言 -->
            <select
                title="Choice language"
                value={get_default_language()}
                on:change={update_cur_language}
            >
                {#each support_languages as lang}
                    <option value={lang.language_key}>
                        <span>{lang.display_name}</span>
                    </option>
                {/each}
            </select>
        </div>
    </div>
    <div class="markdown-box" style="color: {$text_color}">
        {@html $html_content}
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .markdown {
        border-top: 1px solid var(--border-color);
        margin-bottom: 2em;
    }

    .markdown-box {
        width: 80%;
        max-width: $content-max-width;
        margin: 0 auto;
    }

    .markdown-toolbar {
        display: flex;
        justify-content: space-between;
        align-items: baseline;
        flex-direction: row;
        color: mix($bg-color, $text-color, 40%);

        // 尺寸
        width: 80%;
        max-width: $header-max-width;
        margin: 0 auto;

        // 颜色和padding
        padding-top: 0.25em;
        padding-bottom: 0.5em;
    }

    .reader-tools,
    .reader-language {
        display: flex;

        flex-direction: row;
        align-items: baseline;
    }

    .reader-tools button,
    .reader-language select {
        outline: 0;
        display: block;

        // 边距
        margin: 0;
        padding-top: 0.08em;
        padding-bottom: 0.08em;
        padding-left: 0.6em;
        padding-right: 0.6em;

        // 边框
        border: none;
        border-left: 2px solid var(--border-color);

        // 颜色和背景色
        color: var(--button-color);
        background: none;

        &:first-child {
            border: none;
        }

        &:hover {
            background-color: var(--button-hover-bg-color);
        }

        &:active {
            background-color: var(--button-actived-bg-color);
        }
    }

    .reader-language select {
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

    @media screen and (width < 720px) {
        .markdown-toolbar,
        .markdown-box {
            width: 95%;
            max-width: initial;
        }
    }
</style>
