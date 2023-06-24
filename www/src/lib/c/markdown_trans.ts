import generate_toc from './markdown_toc'
import type { Writable } from 'svelte/store'

/**
* 内建支持的语言key
*/
export type BuildinLanguageKey = 'zh' | 'en'

/**
 * 所有支持的语言Key
 */
export type LanguageKey = BuildinLanguageKey

/**
 * 语言项
 */
export type Language = {
    language_key: LanguageKey
    display_name: string
}

/**
* 内容
*/
export type MarkdownContent = {
    html: string
    toc: { level: string; content: string }[]
}

/**
 * 内建的语言支持列表
 */
export type BuildinContent = Record<BuildinLanguageKey, MarkdownContent>

/**
 * 取得支持的语言列表
 */
export function get_support_languages(build_in: BuildinContent): Language[] {
    const langs: Language[] = []
    // 内建的语言
    for (const build_in_item of Object.keys(build_in)) {
        const key = build_in_item as LanguageKey
        const name = language_display_name(key)
        langs.push({ language_key: key, display_name: name })
    }
    return langs
}

/**
 * 加载内建的内容
 */
export function get_build_in_html(language: BuildinLanguageKey, build_in: BuildinContent): string {
    const content = build_in[language]
    return generate_toc(content.html, content.toc)
}

/**
 * 取得翻译出来的html
 */
export default function get_content_html(result: Writable<string>, language: LanguageKey, build_in: BuildinContent) {
    const keys = [...Object.keys(build_in)]
    if (keys.findIndex(s => s === language) != -1) {
        const html = get_build_in_html(language as BuildinLanguageKey, build_in)
        result.set(html)
    }
    else {
        // 不可达branch
        result.set('The language is not supported.')
    }
}

/**
 * 取得语言的名称
 */
function language_display_name(language: LanguageKey): string {
    const lut = new Map<LanguageKey, string>([
        ['zh', '简体中文'],
        ['en', 'English']
    ])
    return lut.get(language) ?? language.toLowerCase()
}
