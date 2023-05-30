import { sveltekit } from '@sveltejs/kit/vite'
import { defineConfig } from 'vite'
import wasm from 'vite-plugin-wasm'
import * as Markdown from 'vite-plugin-markdown'
import MarkdownIt from 'markdown-it'
import topLevelAwait from 'vite-plugin-top-level-await'

export default defineConfig({
    plugins: [
        sveltekit(),
        wasm(),
        topLevelAwait(),
        Markdown.plugin({
            mode: [Markdown.Mode.HTML, Markdown.Mode.TOC],
            markdownIt: (() => {
                const markdown = new MarkdownIt({
                    html: true
                })
                // 超链接的addr
                markdown.core.ruler.push('toc-hyper', state => {
                    state.tokens.forEach((token, index, self) => {
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
                            // 替换处理为合适的html标签
                            const id_name = ((s: string) => s.replace(/[<>&" ]/g, (c: string) => {
                                const lut = new Map([
                                    ['<', '&lt;'],
                                    ['>', '&gt;'],
                                    ['&', '&amp;'],
                                    ['"', '&quot;'],
                                    [' ', '_']
                                ])
                                return lut.get(c) ?? c
                            }))(content)
                            // section的id
                            const section_id = `section-${id_name}`
                            // push attr
                            token.attrPush(['id', section_id])
                        }
                    })
                })
                markdown.enable(['toc-hyper'])
                return markdown
            })()
        })
    ],
    server: {
        fs: {
            allow: ['./wasm/target'],
        },
    }
})
