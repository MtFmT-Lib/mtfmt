// SPDX-License-Identifier: LGPL-3.0
import MtfmtWASM from './mtfmt'

/**
 * 取得库的wrapper版本号
 * 
 * @returns 版本号
 */
export function wrap_version(): number {
    return MtfmtWASM.mstr_wasm_version()
}

/**
 * 字符串(inner)
 */
class MtFmtRawString {
    private handler: number

    constructor() {
        // 申请内存
        const hstr = MtfmtWASM.mstr_wasm_new_string()
        if (hstr === 0) {
            throw WebAssembly.RuntimeError('Cannot allocate memory for MString object')
        }
        // 初始化字符串
        MtfmtWASM.mstr_create(hstr, 0)
        // 记录handle
        this.handler = hstr
    }

    public get_handle(): number {
        return this.handler
    }
}

/**
 * 字符串类
 */
export class MtFmtString {
    private raw_str: MtFmtRawString

    constructor() {
        // 创建字符串
        const raw_obj = new MtFmtRawString()
        // 析构
        const registry = new FinalizationRegistry((val: number) => {
            MtfmtWASM.mstr_free(val)
            MtfmtWASM.mstr_wasm_free_string(val)
        })
        // 析构
        registry.register(raw_obj, raw_obj.get_handle())
        // <=
        this.raw_str = raw_obj
    }

    /**
     * 添加一个字符
     * 
     * @param ch : 需要添加的字符
     */
    public append_char(ch: string) {
        const val = ch.codePointAt(0) ?? 0
        const hstr = this.raw_str.get_handle()
        MtfmtWASM.mstr_append(hstr, val)
    }

    /**
     * 取得字符串长度
     */
    public get_length(): number {
        const hstr = this.raw_str.get_handle()
        return MtfmtWASM.mstr_wasm_string_len(hstr)
    }

    /**
     * 取得字符串内容
     * 
     * @returns js字符串
     */
    public get_string(): string {
        return MtfmtWASM.mstr_as_cstr(this.raw_str.get_handle())
    }
}

export function test() {
    console.log(MtfmtWASM)
    console.log(wrap_version())

    const cstr = new MtFmtString()
    console.log(cstr.get_length())
    console.log(cstr.append_char('@'))
    console.log(cstr.get_length())
}
