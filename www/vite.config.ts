import { sveltekit } from '@sveltejs/kit/vite'
import { defineConfig } from 'vite'
import wasm from 'vite-plugin-wasm'
import * as Markdown from 'vite-plugin-markdown'
import topLevelAwait from 'vite-plugin-top-level-await'
import hljs from 'highlight.js'
import MarkdownIt from 'markdown-it'
import { katex } from '@mdit/plugin-katex'

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
        html: true,
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
    markdown.core.ruler.push('toc-hyper', state => {
        state.tokens.forEach((token, index, self) => {
            // 段落标记
            if (token.tag.match(/h[0-9]/g) && token.nesting == 1) {
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
            // 插入断行(如果需要)
            if (token.type === 'inline' && token.content.indexOf('`') != -1) {
                token.children?.forEach(token => {
                    if (token.type === 'code_inline') {
                        token.attrPush(['class', 'wrap_code_inline'])
                        // 理论上不应该存在children
                        if (token.children !== null) {
                            throw 'Token children is NOT null.'
                        }
                        // 处理内容, 插入断行
                        const content = token.content
                        token.content = insert_zero_space_to_code(content)
                    }
                })
            }
        })
    })
    markdown.enable(['toc-hyper'])
    return markdown
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
 * 插入0宽度空格方便进行断行
 */
function insert_zero_space_to_code(str: string): string {
    const arr = str.split('_')
    return arr.join('_\u200B')
}
