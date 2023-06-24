// SPDX-License-Identifier: LGPL-3.0
import { writable } from 'svelte/store'

/**
 * 主题的语言属性名
 */
const THEME_LANGUAGE_ATTRIBUTE = 'page-lang'

/**
 * 支持的语言
 */
export type Language = string

/**
 * 语言信息的类型
 */
export type LanguageInfo = {
    language: Language
}

/**
 * 设置语言的style prop
 */
export function set_language_attrs(lang: Language) {
    language_info.update((store) => ({ ...store, language: lang }))
}

/**
 * 更改language
 */
export function change_language(doc: Document, new_language: LanguageInfo) {
    const language = new_language.language
    const root_element = doc.documentElement
    root_element.setAttribute(THEME_LANGUAGE_ATTRIBUTE, language)
}

/**
 * 语言信息
 */
const language_info = writable<LanguageInfo>({
    language: 'en'
})

export default language_info
