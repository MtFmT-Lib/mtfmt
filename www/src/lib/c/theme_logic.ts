// SPDX-License-Identifier: LGPL-3.0
import type { SystemTheme, ThemeInfo } from './theme_storager'
import { STRONGE_THEME_KEY, AVALIABLE_THEMES } from './theme_storager'
import { THEME_MEDIA, update_system_theme } from './theme_storager'

/**
 * 主题的属性名
 */
const THEME_ATTRIBUTE = 'page-theme'

/**
 * 更改theme
 */
export function change_theme(doc: Document, new_theme: ThemeInfo) {
    const theme_name = new_theme.following
        ? new_theme.system_theme
        : new_theme.cur_theme
    // 设置属性
    const root_element = doc.documentElement
    root_element.setAttribute(THEME_ATTRIBUTE, theme_name)
    root_element.style.setProperty('color-scheme', theme_name)
}

/**
 * 设置默认主题
 */
export function set_default_theme(doc: Document, new_theme: ThemeInfo) {
    const theme = localStorage.getItem(STRONGE_THEME_KEY) ?? new_theme.system_theme
    if (theme in AVALIABLE_THEMES) {
        const root_element = doc.documentElement
        root_element.setAttribute(THEME_ATTRIBUTE, theme)
        root_element.style.setProperty('color-scheme', theme)
    }
}

/**
 * 注册窗口相关的event
 */
export function register_window_event(window: Window) {
    const media = window.matchMedia(THEME_MEDIA)
    // 增加监听事件
    // 使用该方法支持iOS和旧浏览器
    // see also:
    // https://developer.mozilla.org/zh-CN/docs/Web/API/MediaQueryList/addListener
    let select_new = true
    try {
        media.addEventListener('change', handle_media_query)
    } catch {
        select_new = false
        media.addListener(handle_media_old)
    }
    // 刷新系统当前的theme
    update_current_system_theme(media.matches)
    // ret
    return {
        destroy: () => {
            if (select_new) {
                media.removeEventListener('change', handle_media_query)
            } else {
                media.removeListener(handle_media_old)
            }
        },
    }
}

/**
 * 刷新系统的theme
 */
function update_current_system_theme(media_match: boolean) {
    const sys_theme: SystemTheme = media_match ? 'dark' : 'light'
    update_system_theme(sys_theme)
}

/**
 * 检查并变更主题
 */
function handle_media_query(earg: MediaQueryListEvent) {
    update_current_system_theme(earg.matches)
}

/**
 * 兼容老版本的media handler
 */
function handle_media_old(...args: any) {
    return handle_media_query(args)
}
