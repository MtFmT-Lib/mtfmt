import { sveltekit } from '@sveltejs/kit/vite'
import { defineConfig } from 'vite'
import wasm from 'vite-plugin-wasm'
import * as Markdown from 'vite-plugin-markdown'
import topLevelAwait from 'vite-plugin-top-level-await'
import hljs from 'highlight.js'
import MarkdownIt from 'markdown-it'
import { textVide } from 'text-vide'
import { katex } from '@mdit/plugin-katex'
import Token from 'markdown-it/lib/token'

export default defineConfig({
    plugins: [
        sveltekit(),
        wasm(),
        topLevelAwait(),
        Markdown.plugin({
            mode: [Markdown.Mode.HTML, Markdown.Mode.TOC],
            markdownIt: markdown_preprocess()
        })
    ],
    server: {
        fs: {
            allow: ['./wasm/target'],
        },
    }
})

/**
 * markdown预处理
 */
function markdown_preprocess(): MarkdownIt {
    const markdown = new MarkdownIt({
        html: false,
        highlight: (str, lang) => {
            if (lang) {
                return highlight_process(str, lang)
            } else {
                return str
            }
        }
    })
    // katex
    markdown.use(katex)
    // 超链接的addr
    markdown.use(plugin_section_id)
    // 断行
    markdown.use(plugin_line_break)
    // bionic reading
    markdown.use(plugin_bionic_reading)
    return markdown
}

/**
 * 段落id
 */
function plugin_section_id(md: MarkdownIt): void {
    md.core.ruler.push('toc-hyper', state => {
        state.tokens.forEach((token, index, self) => {
            if (token.tag.match(/h[0-9]/g) && token.nesting === 1) {
                // <hxx>认为是一个section</hxx>
                // 检查下一个内容
                const content = ((pos, arr) => {
                    if (pos >= arr.length) {
                        return ''
                    }
                    const token = arr[pos]
                    return token.content
                })(index + 1, self)
                // section的id
                const section_id = as_id_name(content)
                // push attr
                token.attrPush(['id', section_id])
            }
        })
    })
    md.enable(['toc-hyper'])
}

/**
 * 转换为合适的id名
 */
function as_id_name(content: string): string {
    const name = content.replace(/[<>&". -:]/g, (c: string) => {
        const lut = new Map([
            ['<', '&lt;'],
            ['>', '&gt;'],
            ['&', '&amp;'],
            ['"', '&quot;'],
            [' ', '_'],
            ['-', '_'],
            [':', '_'],
            ['.', '_']
        ])
        return lut.get(c) ?? c
    })
    return 'section_' + name
}

/**
 * 断行
 */
function plugin_line_break(md: MarkdownIt): void {
    md.core.ruler.push('code-line-break', state => {
        state.tokens.forEach(token => {
            token.children?.forEach(token => {
                if (token.type === 'code_inline') {
                    token.attrPush(['class', 'wrap_code_inline'])
                    // 理论上不应该存在children
                    if (token.children !== null) {
                        throw 'Token children is NOT null.'
                    }
                    // 处理内容, 插入断行
                    const content = token.content
                    token.content = insert_line_break_marker(content)
                }
            })
        })
    })
    md.enable(['code-line-break'])
}

/**
 * 插入0宽度空格方便进行断行
 */
function insert_line_break_marker(str: string): string {
    const arr = str.split('_')
    return arr.join('_\u200B')
}

/**
 * bionic-reading
 */
function plugin_bionic_reading(md: MarkdownIt): void {
    md.core.ruler.push('bionic-reading', state => {
        let process_index = 0
        state.tokens.forEach((token, index, self) => {
            if (token.type === 'paragraph_open' && index >= process_index) {
                const begin_index = index + 1
                // 找到end
                let it = process_index + 1
                while (it < self.length && self[it].type !== 'paragraph_close') {
                    it += 1
                }
                process_index = it
                const end_index = it
                // begin_index ... end_index即是一段的内容, 处理段落
                const paragraph = self.slice(begin_index, end_index)
                paragraph.forEach(token => {
                    const children: Token[] = []
                    // 处理文本
                    token.children?.forEach(token => {
                        if (token.type === 'text') {
                            if (token.children != null) {
                                throw 'Markdown parser: Cannot process a text with children'
                            }
                            const result = add_bionic_reading(token.content)
                            children.push(...result)
                        } else {
                            children.push(token)
                        }
                    })
                    // 更新children
                    token.children = children
                })
            }
        })
    })
    md.enable(['bionic-reading'])
}

/**
 * 从text生成token stream, 插入bionic-reading的标签
 */
function add_bionic_reading(str: string): Token[] {
    const BEGIN_SPEC = '\ufff0'
    const END_SPEC = '\ufff1'
    const HIGHLIGHT_BEGIN = '<span class="bio-reading-bold">'
    const HIGHLIGHT_END = '</span>'
    // 处理内容
    const output_str = bionic_reading_process(str, BEGIN_SPEC, END_SPEC)
    // 生成token
    let buffer = ''
    let nesting_check = 0
    const result: Token[] = []
    for (const ch of output_str) {
        if (ch !== BEGIN_SPEC && ch !== END_SPEC) {
            buffer += ch
        } else if (buffer.length > 0) {
            // flush
            result.push(make_text_token(buffer))
            buffer = ''
        }
        // 处理begin和end
        if (ch === BEGIN_SPEC) {
            nesting_check += 1
            result.push(make_html_token(HIGHLIGHT_BEGIN))
        } else if (ch === END_SPEC) {
            nesting_check -= 1
            result.push(make_html_token(HIGHLIGHT_END))
        }
    }
    if (buffer.length > 0) {
        result.push(make_text_token(buffer))
    }
    if (nesting_check !== 0) {
        throw 'Nesting check fault'
    }
    return result
}

/**
 * bionic_reading处理
 */
function bionic_reading_process(str: string, split_beg: string, split_end: string): string {
    if (str.match(/\[toc\]|\[TOC\]/) === null) {
        return textVide(str, {
            ignoreHtmlTag: true,
            sep: [split_beg, split_end]
        })
    } else {
        return str
    }
}

/**
 * 高亮处理
 */
function highlight_process(str: string, lang: string): string {
    if (hljs.getLanguage(lang)) {
        const html = hljs.highlight(str, { language: lang }).value
        return '<pre class="hljs"><code>' + html + '</code></pre>'
    } else {
        return str
    }
}

/**
 * 构造text的标签
 */
function make_text_token(str: string): Token {
    const token = new Token('text', '', 0)
    token.block = false
    token.hidden = false
    token.attrs = null
    token.map = null
    token.level = 0
    token.children = null
    token.content = str
    token.markup = ''
    token.info = ''
    token.meta = null
    token.block = false
    token.hidden = false
    return token
}

/**
 * 构造HTML的标签
 */
function make_html_token(tag: string): Token {
    const token = new Token('html_inline', '', 0)
    token.attrs = null
    token.map = null
    token.level = 0
    token.children = null
    token.content = tag
    token.markup = ''
    token.info = ''
    token.meta = null
    token.block = false
    token.hidden = false
    return token
}
