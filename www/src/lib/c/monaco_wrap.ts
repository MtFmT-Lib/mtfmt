// SPDX-License-Identifier: LGPL-3.0
import type monaco from 'monaco-editor'
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker'
import theme_info, { type ThemeInfo } from './theme_storager'

/**
 * 编辑器
 */
let monaco_editor: monaco.editor.IStandaloneCodeEditor | null = null

/**
 * theme info change订阅
 */
theme_info.subscribe(val => {
    monaco_editor?.updateOptions({
        theme: get_monaco_theme_name(val)
    })
})

/**
 * Worker的map
 */
export type EditorWorkerMap = Map<string, () => Promise<Worker> | Worker>

/**
 * 异步加载编辑器
 */
export async function load_editor_module(
    div_element: HTMLDivElement,
    workers: EditorWorkerMap,
    theme_info: ThemeInfo
): Promise<void> {
    self.MonacoEnvironment = {
        getWorker: (workerId: string, label: string) => {
            return (workers.get(label) ?? (() => new editorWorker()))()
        },
        // @ts-ignore
        // see also: https://github.com/microsoft/vscode/issues/184732
        createTrustedTypesPolicy: undefined,
    }
    // 加载
    const Monaco = await import('monaco-editor/esm/vs/editor/editor.api')
    // 创建元素
    const editor = Monaco.editor.create(div_element, {
        value: 'function x() {\n    console.log("Hello world!");\n}',
        language: 'javascript',
        theme: get_monaco_theme_name(theme_info)
    })
    monaco_editor = editor
}

/**
 * 要求编辑器重新布局
 */
export function re_layout() {
    monaco_editor?.layout()
}

/**
 * 销毁编辑器
 */
export function destroy_instance() {
    monaco_editor?.dispose()
}

/**
 * 取得主题
 */
function get_monaco_theme_name(theme_info: ThemeInfo) {
    const cur_theme = theme_info.cur_theme
    const sys_theme = theme_info.system_theme
    const theme_sty = cur_theme === 'sys' ? sys_theme : cur_theme
    // dark使用vs-dark, 其余情况使用vs
    if (theme_sty === 'dark') {
        return 'vs-dark'
    } else {
        return 'vs'
    }
}
