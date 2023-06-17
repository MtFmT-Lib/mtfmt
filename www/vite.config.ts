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
import * as fs from 'node:fs'
import path from 'node:path'

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
    // 嵌入图片
    markdown.use(plugin_embed_image)
    // 超链接的addr
    markdown.use(plugin_section_id)
    // 断行
    markdown.use(plugin_line_break)
    // bionic reading
    markdown.use(plugin_bionic_reading)
    return markdown
}

/**
 * 嵌入图片
 */
function plugin_embed_image(md: MarkdownIt): void {
    md.core.ruler.push('embed-image', state => {
        state.tokens.forEach(token => {
            if (token.type === 'inline') {
                if (token.children === null) {
                    return
                }
                if (token.children.length !== 1) {
                    return
                }
                if (token.children[0].type !== 'image') {
                    return
                }
                // 图片转换为base64
                const img_child = token.children[0]
                const img_path = img_child.attrGet('src')
                if (img_path !== null) {
                    const img_base64 = readfile_as_base64(img_path)
                    img_child.attrSet('src', img_base64)
                }
                // 添加图注
                // 总而言之它以一种很奇怪的方式跑了起来...
                const img_alt = img_child.attrGet('alt') ?? ''
                let img_alt_content: string
                if (img_alt.length > 0) {
                    img_alt_content = img_alt
                }
                else {
                    img_alt_content = img_child.content
                }
                const text_token = make_html_token(img_alt_content)
                token.children.push(make_html_token('<div class="img-info">'))
                token.children.push(text_token)
                token.children.push(make_html_token('</div>'))
                // 添加图片id
                img_child.attrSet('id', as_figure_id_name(img_alt_content))
            }
        })
    })
    md.enable(['embed-image'])
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
                const section_id = as_section_id_name(content)
                // push attr
                token.attrPush(['id', section_id])
            }
        })
    })
    md.enable(['toc-hyper'])
}

/**
 * 转换为合适的id名
 * 
 * 该函数会优先考虑 [0-9] { `.` [0-9]} 的范式生成id
 * 否则会生成完整的内容
 */
function as_section_id_name(content: string): string {
    const patt = content.match(/[0-9](\.[0-9]+)*/g)
    const lut = new Set([
        '<', '>', '&', '"', '\'', ' ', '-', ':', '.'
    ])
    const section_id = patt ? patt[0] : content
    const name = section_id.replace(/[<>&". -:]/g, (c: string) => {
        return lut.has(c) ? '_' : c
    })
    return 'section_' + name.toLowerCase()
}

/**
 * 转换为合适的id名
 * 
 * 该函数会优先考虑 Figure[0-9] { `.` [0-9]} 的范式生成id
 * 否则会生成完整的内容
 */
function as_figure_id_name(content: string): string {
    const patt = content.match(/(F|f)igure( *)([0-9](\.[0-9]+)*)/)
    const lut = new Set([
        '<', '>', '&', '"', '\'', ' ', '-', ':', '.'
    ])
    const figure_id = patt ? patt[3].toLowerCase() : content.toLowerCase()
    const name = figure_id.replace(/[<>&". -:]/g, (c: string) => {
        return lut.has(c) ? '_' : c
    })
    return 'figure_' + name
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
 * 读取文件并转为base64
 */
function readfile_as_base64(file: string): string {
    const src_path = path.resolve('./src/texts')
    const full_path = path.resolve(src_path, file)
    const file_ext_name = path.extname(full_path)
    // 读取并转换base64
    const fs_data = fs.readFileSync(full_path)
    const base64 = fs_data.toString('base64')
    // 转换mime type
    const mime_lut = new Map([
        ['.gif', 'image/gif'],
        ['.png', 'image/png'],
        ['.jpg', 'image/jpeg'],
        ['.jpeg', 'image/jpeg'],
        ['.webp', 'image/webp'],
    ])
    const mime_type = mime_lut.get(file_ext_name)
    if (mime_type) {
        return 'data:' + mime_type + ';base64,' + base64
    } else {
        throw `No mime type for extname ${file_ext_name}`
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
