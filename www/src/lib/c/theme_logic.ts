// SPDX-License-Identifier: LGPL-3.0
import { THEME_MEDIA, update_system_theme, } from './theme_storager'
import { get_storager_theme } from './theme_storager'
import type { SystemTheme, ThemeInfo, Theme } from './theme_storager'

/**
 * 主题的属性名
 */
const THEME_ATTRIBUTE = 'page-theme'

/**
 * 更改theme
 */
export function change_theme(doc: Document, new_theme: ThemeInfo) {
    const theme = new_theme.following
        ? new_theme.system_theme
        : new_theme.cur_theme
    // 设置属性
    set_theme_attr(doc, theme)
}

/**
 * 设置默认主题
 */
export function set_default_theme(doc: Document, new_theme: ThemeInfo) {
    const theme = get_storager_theme(new_theme)
    set_theme_attr(doc, theme)
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
        media.addListener(handle_media_query)
    }
    // 刷新系统当前的theme
    update_current_system_theme(media.matches)
    // ret
    return {
        destroy: () => {
            if (select_new) {
                media.removeEventListener('change', handle_media_query)
            } else {
                media.removeListener(handle_media_query)
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
 * 设置主题的属性
 */
function set_theme_attr(doc: Document, theme: Theme) {
    const root_element = doc.documentElement
    root_element.setAttribute(THEME_ATTRIBUTE, theme)
    root_element.style.setProperty('color-scheme', theme)
}
