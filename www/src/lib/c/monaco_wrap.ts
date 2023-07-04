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
 * 语言支持
 */
export type EditorLanguageSupport = {
    /**
     * 语言支持信息
     */
    language: monaco.languages.ILanguageExtensionPoint,

    /**
     * Token
     */
    token_provider?: monaco.languages.IMonarchLanguage
    | monaco.Thenable<monaco.languages.IMonarchLanguage>,

    /**
     * 自动完成
     */
    comp_provider?: monaco.languages.CompletionItemProvider
}

/**
 * 异步加载编辑器
 */
export async function load_editor_module(
    div_element: HTMLDivElement,
    workers: EditorWorkerMap,
    theme_info: ThemeInfo,
    languages?: EditorLanguageSupport[]
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
    const monaco = await import('monaco-editor/esm/vs/editor/editor.api')
    // 注册语言
    if (languages) {
        for (const language of languages) {
            const language_id = language.language.id
            // 注册语言
            monaco.languages.register(language.language)
            // token provider
            if (language.token_provider) {
                monaco.languages.setMonarchTokensProvider(
                    language_id, language.token_provider
                )
            }
            // 自动完成
            if (language.comp_provider) {
                monaco.languages.registerCompletionItemProvider(
                    language_id, language.comp_provider
                )
            }
        }
    }
    // 创建元素
    const editor = monaco.editor.create(div_element, {
        value: 'function x() {\n    console.log("Hello world!");\n}',
        language: 'javascript',
        theme: get_monaco_theme_name(theme_info),
        minimap: {
            enabled: false
        }
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
