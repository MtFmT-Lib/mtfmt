<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Picker from './Picker.svelte'
    import Toggler from './Toggler.svelte'
    import { writable } from 'svelte/store'
    import get_html from './markdown_trans'
    import * as Mts from './markdown_trans'
    import { curry_2 } from '$lib/fp/fun'
    import { into_boolean } from '$lib/fp/cast'
    import { apply_svelte } from '$lib/fp/apply'
    import * as Storager from '$lib/local_storager'
    import { set_language_attrs } from './theme_lang'
    import theme_info, { type Theme } from './theme_storager'
    import { set_theme, get_storager_theme } from './theme_storager'

    /**
     * 文本颜色
     */
    type TextColor = 'inherit' | 'var(--weak-color)'

    /**
     * 内容
     */
    export let contents: Mts.BuildinContent

    /**
     * apply函数
     */
    const event_fun = curry_2(apply_svelte)

    /**
     * 支持的语言
     */
    let support_languages = Mts.get_support_languages(contents)

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
    let html_content = writable<string>(Mts.get_build_in_html('en', contents))

    /**
     * html内容目录
     */
    let html_content_toc = writable<string>(
        Mts.get_build_in_toc('en', contents)
    )

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
        get_html(html_content, html_content_toc, language, contents)
    })

    /**
     * 切换bio reading
     */
    function set_bio_mode(arg: CustomEvent<{ state: boolean }>) {
        const enable = arg.detail.state
        enable_bio_reader.set(enable)
        Storager.write_local_storager(BIO_READING_ITEM_KEY, enable)
    }

    /**
     * 更新选中的主题
     */
    function update_cur_theme(arg: { value: string }) {
        const cur_theme = arg.value as Theme
        set_theme(cur_theme)
    }

    /**
     * 更新选中的语言
     */
    function update_cur_language(arg: { value: string }) {
        const language = arg.value as Mts.LanguageKey
        cur_language.set(language)
        Storager.write_local_storager(LANGUAGE_ITEM_KEY, language)
    }

    /**
     * 取得默认的语言
     */
    function get_default_language(): Mts.LanguageKey {
        return Storager.read_local_storager(LANGUAGE_ITEM_KEY)
            .map((s) => s as Mts.LanguageKey)
            .or_map(() => {
                let language: Mts.LanguageKey
                if (typeof navigator === 'undefined') {
                    language = 'en'
                } else {
                    const lut = new Map<string, Mts.LanguageKey>([
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
    <div class="markdown-inner">
        <div class="markdown-index">{@html $html_content_toc}</div>
        <div class="markdown-content">
            <div class="markdown-toolbar">
                <div class="reader-tools">
                    <!-- bio 阅读 -->
                    {#if $cur_language === 'en'}
                        <div class="reader-tool-item">
                            <Toggler
                                title="Disable the bio-reading mode"
                                checked={$enable_bio_reader}
                                on:toggle={set_bio_mode}
                            >
                                <span>B</span>
                            </Toggler>
                        </div>
                    {/if}
                </div>
                <div class="reader-language">
                    <!-- 主题 -->
                    <div class="reader-tool-item">
                        <Picker
                            title="Select the theme"
                            items={$theme_info.themes.map((v) => ({
                                value: v,
                                display_name: v.toUpperCase(),
                            }))}
                            selected_item={get_storager_theme()}
                            on:picknew={event_fun(update_cur_theme)}
                        />
                    </div>
                    <!-- 语言 -->
                    <div class="reader-tool-item">
                        <Picker
                            title="Choice language"
                            items={support_languages.map((v) => ({
                                value: v.language_key,
                                display_name: v.display_name,
                            }))}
                            selected_item={get_default_language()}
                            on:picknew={event_fun(update_cur_language)}
                        />
                    </div>
                </div>
            </div>
            <div class="markdown-box" style="color: {$text_color}">
                {@html $html_content}
            </div>
        </div>
        <div style="clear: both" />
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .markdown {
        border-top: 1px solid var(--border-color);
    }

    .markdown-inner {
        width: 80%;
        max-width: $content-max-width;
        margin: 0 auto;

        @media screen and (width < $lg) {
            width: 100%;
        }
    }

    .markdown-index {
        float: left;
        width: 25%;
        box-sizing: border-box;
        padding: 1em 1em;

        @media screen and (width < $md) {
            display: none;
        }

        @media screen and (width > $xl) {
            width: 360px;
        }
    }

    .markdown-content {
        margin-left: 25%;
        max-width: $content-text-max-width;
        border-left: 1px solid var(--border-color);

        @media screen and (width < $md) {
            border: none;
            padding: 0;
            margin: 0;
            width: 100%;
        }

        @media screen and (width > $xl) {
            margin-left: 360px;
        }
    }

    .markdown-box {
        box-sizing: border-box;
        padding: 1em 1.5em;

        @media screen and (width < $md) {
            padding: 1em 0.8em;
        }
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
    }

    .reader-tools,
    .reader-language {
        display: flex;

        flex-direction: row;
        align-items: baseline;
    }

    .reader-tool-item {
        border-left: 2px solid var(--border-color);

        &:first-child {
            border: none;
        }
    }
</style>
