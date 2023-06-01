// SPDX-License-Identifier: LGPL-3.0

/**
 * 将第一次出现的<p>[toc]</p>替换并生成目录
 */
export default function generate_toc(
    html_raw: string,
    toc: { level: string; content: string }[]
): string {
    const len = '<p>[toc]</p>'.length
    const pos = html_raw.search(/(<p>\[(toc)|(TOC)\]<\/p>)/g)
    if (pos == -1) {
        return html_raw
    } else {
        const part1 = html_raw.slice(0, pos)
        const part2 = html_raw.slice(pos + len)
        const toc_html = generate_toc_helper(toc)
        return part1 + toc_html + part2
    }
}

/**
 * 生成目录
 *
 * @param toc: 目录
 *
 * @returns: HTML RAW, 表示目录, 拥有父元素<div class=".toc">
 */
function generate_toc_helper(toc: { level: string; content: string }[]): string {
    let html_raw = ''
    let tag_count = 0
    let last_level = 0
    const MAX_LEVEL = 3
    for (const toc_item of toc) {
        const level = parseInt(toc_item.level)
        const toc_content = toc_item.content
        if (level > MAX_LEVEL) {
            continue
        }
        // <ul>
        if (last_level < level) {
            tag_count += 1
            html_raw += '<ul class="toc-tree">'
        }
        // </ul>
        if (last_level > level) {
            tag_count -= 1
            html_raw += '</ul>'
        }
        // 添加内容
        const id_name = as_id_name(toc_content)
        const id_label = `
            <a class="toc-hyper-link"
               href="#${id_name}"
               title="${toc_content}">
                ${toc_content}
            </a>`
        html_raw += `<li class="toc-level${level}-content">${id_label}</li>`
        // 记录目录level
        last_level = level
    }
    // </ul> 闭合
    if (tag_count == 1) {
        html_raw += '</ul>'
    }
    return html_raw
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
