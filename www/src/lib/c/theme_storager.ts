// SPDX-License-Identifier: LGPL-3.0
import { writable } from 'svelte/store'
import { into_boolean } from '$lib/fp/cast'
import { read_local_storager } from '$lib/local_storager'
import { write_local_storager } from '$lib/local_storager'

/**
 * 系统的theme
 */
export type SystemTheme = 'light' | 'dark'

/**
 * 主题
 */
export type Theme = SystemTheme | 'warm' | 'sys'

/**
 * 可用的主题列表
 */
export const AVALIABLE_THEMES: Theme[] = ['light', 'dark', 'warm', 'sys']

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
 * Stronge key的名称
 */
export const STRONGE_THEME_KEY = 'theme'

/**
 * Stronge key (following)的名称
 */
export const STRONGE_FOLLOWING_KEY = 'theme-following'

/**
 * 默认主题
 */
export const DEFAULT_THEME: SystemTheme = 'light'

/**
 * 取得存储记录的theme
 */
export function get_storager_theme(theme_info?: ThemeInfo): Theme {
    // 是否跟随system
    const following = read_local_storager(STRONGE_FOLLOWING_KEY)
        .map(into_boolean)
        .or(false)
    // 默认的theme
    const default_theme = theme_info?.system_theme ?? DEFAULT_THEME
    if (following) {
        return default_theme
    } else {
        return read_local_storager<Theme>(STRONGE_THEME_KEY).or(default_theme)
    }
}

/**
 * 设置主题
 * 
 * @param cur_theme: 主题名
 */
export function set_theme(cur_theme: Theme): void {
    if (cur_theme === 'sys') {
        write_local_storager(STRONGE_FOLLOWING_KEY, true)
        return theme_info.update((store) => ({ ...store, following: true }))
    } else {
        write_local_storager(STRONGE_THEME_KEY, cur_theme)
        write_local_storager(STRONGE_FOLLOWING_KEY, false)
        return theme_info.update((store) => ({ ...store, cur_theme, following: false }))
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
    cur_theme: get_storager_theme(),
    following: read_local_storager(STRONGE_FOLLOWING_KEY).map(into_boolean).or(false),
    system_theme: 'light',
    themes: AVALIABLE_THEMES
})

export default theme_info
