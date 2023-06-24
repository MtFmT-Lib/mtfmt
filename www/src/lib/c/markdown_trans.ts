import generate_toc from './markdown_toc'

/**
* 内建支持的语言key
*/
export type BuildinLanguageKey = 'zh' | 'en'

/**
 * 所有支持的语言Key
 */
export type LanguageKey = BuildinLanguageKey | 'jp'

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
 * API支持的语言
 */
export const API_SUPPORTS: LanguageKey[] = ['jp']

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
    // API支持的语言
    for (const api_item of API_SUPPORTS) {
        const key = api_item
        const name = language_display_name(api_item)
        langs.push({ language_key: key, display_name: name })
    }
    return langs
}

/**
 * 取得翻译过来的内容
 */
export function get_content(language: LanguageKey, build_in: BuildinContent): MarkdownContent {
    const keys = [...Object.keys(build_in)]
    if (keys.findIndex(s => s === language) != -1) {
        return build_in[language as BuildinLanguageKey]
    } else {
        return translate(language, build_in['en'])
    }
}

/**
 * 取得翻译出来的html
 */
export default function get_content_html(language: LanguageKey, build_in: BuildinContent): string {
    const markdown = get_content(language, build_in)
    return generate_toc(markdown.html, markdown.toc)
}

/**
 * 取得语言的名称
 */
function language_display_name(language: LanguageKey): string {
    const lut = new Map<LanguageKey, string>([
        ['zh', '简体中文'],
        ['en', 'English'],
        ['jp', '日本語']
    ])
    return lut.get(language) ?? language.toLowerCase()
}

/**
 * 进行翻译
 */
function translate(language: LanguageKey, content: MarkdownContent): MarkdownContent {
    const html_content = content.html
    const tocs_content = content.toc
    // 增加机翻提示
    const trans_hint = translate_hint(language)
    return {
        html: trans_hint + html_content,
        toc: tocs_content
    }
}

/**
 * 取得机器翻译提示
 */
function translate_hint(language: LanguageKey): string {
    return `
        <div class="translate-hint">
            Language "${language_display_name(language)}" is not provided. It was translated by machine
        </div>`
}
