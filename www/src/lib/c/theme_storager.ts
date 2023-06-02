// SPDX-License-Identifier: LGPL-3.0
import { writable } from 'svelte/store'

/**
 * 系统的theme
 */
export type SystemTheme = 'light' | 'dark'

/**
 * 主题
 */
export type Theme = SystemTheme | 'warm'

/**
 * 可用的主题列表
 */
export const AVALIABLE_THEMES: Theme[] = ['light', 'dark', 'warm']

/**
 * Theme信息
 */
export interface ThemeInfo {
    /**
     * 是否追随系统的主题
     */
    following: boolean

    /**
     * 当前主题
     */
    cur_theme: Theme,

    /**
     * 当前系统主题
     */
    system_theme: SystemTheme,

    /**
     * 可用主题
     */
    themes: Theme[]
}

/**
 * Media名
 */
export const THEME_MEDIA = '(prefers-color-scheme: dark)'

/**
 * Stronge key的名
 */
export const STRONGE_THEME_KEY = 'theme'

/**
 * 默认主题
 */
export const DEFAULT_THEME: SystemTheme = 'light'

/**
 * 设置主题
 * 
 * @param cur_theme: 主题名, 留空选择system_theme
 */
export function set_theme(cur_theme?: Theme): void {
    if (cur_theme) {
        localStorage.setItem(STRONGE_THEME_KEY, cur_theme)
        return theme_info.update((store) => ({ ...store, cur_theme, following: false }))
    } else {
        return theme_info.update((store) => ({ ...store, following: true }))
    }
}

/**
 * 更新系统主题名
 * 
 * @param theme: 主题名
 */
export function update_system_theme(theme: SystemTheme): void {
    return theme_info.update((store) => ({ ...store, system_theme: theme }))
}

/**
 * 主题信息
 */
const theme_info = writable<ThemeInfo>({
    following: false,
    cur_theme: 'light',
    system_theme: 'light',
    themes: AVALIABLE_THEMES
})

export default theme_info
