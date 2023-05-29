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
    console.log(toc)
    return '@@TOC@@'
}
