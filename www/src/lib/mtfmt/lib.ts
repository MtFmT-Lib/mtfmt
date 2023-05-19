// SPDX-License-Identifier: LGPL-3.0
import MtfmtWASM from './mtfmt'

/**
 * 库错误
*/
export enum MStrResult {
    MStr_Ok = 0,
    // ERR: 内存错误: 内存分配失败
    MStr_Err_HeapTooSmall = -128,
    // ERR: buff太短
    MStr_Err_BufferTooSmall,
    // ERR: 格式化: 索引太多
    MStr_Err_IndexTooLarge,
    // ERR: 格式化: Parser错误: 未识别的token
    MStr_Err_UnrecognizedToken,
    // ERR: 格式化: Parser错误: Missing `{`
    MStr_Err_MissingLeftBrace,
    // ERR: 格式化: Parser错误: Missing `}`
    MStr_Err_MissingRightBrace,
    // ERR: 格式化: Parser错误: Missing Formatter
    MStr_Err_MissingReplacement,
    // ERR: 格式化: Parser错误: Missing ArgID
    MStr_Err_MissingArgumentID,
    // ERR: 格式化: Parser错误: Missing ArgType
    MStr_Err_MissingArgumentType,
    // ERR: 格式化: Parser错误: 不支持的fill char
    MStr_Err_UnsupportFillChar,
    // ERR: 格式化: Parser错误: fill char后面必须要align
    MStr_Err_MissingAlignAfterFillChar,
    // ERR: 格式化: Parser错误: 不支持的格式化方式
    MStr_Err_UnsupportFormatType,
    // ERR: 格式化: Parser错误: 请求宽度太大
    MStr_Err_WidthTooLarge,
    // ERR: 格式化: Parser错误: 请求的格式化项太多(日期时间)
    MStr_Err_TooMoreChronoItem,
    // ERR: 格式化: Parser错误: 错失格式化的token(日期时间)
    MStr_Err_MissingChronoItemType,
    // ERR: 格式化: Parser错误: 未指定
    MStr_Err_UndefinedParserError,
    // ERR: 格式化: 还未使用过的参数ID
    MStr_Err_UnusedArgumentID,
    // ERR: 格式化: 参数太大
    MStr_Err_InvaildArgumentID,
    // ERR: 格式化: 类型不正确
    MStr_Err_InvaildArgumentType,
    // ERR: 格式化: 内建buffer不够大
    MStr_Err_InternalBufferTooSmall,
    // ERR: 格式化: 不支持的类型
    MStr_Err_UnsupportType,
    // ERR: 格式化: 不支持的量化精度
    MStr_Err_UnsupportQuantBits,
    // ERR: 最后一个的flag
    MStr_Err_Flag_LastOne,
}

/**
 * 取得库的wrapper版本号
 * 
 * @returns 版本号
 */
export function wrap_version(): number {
    return MtfmtWASM.mstr_wasm_version()
}

/**
 * C对象接口
 */
export interface CObject {
    destroy(): void
}

/**
 * 字符串
 */
export class CString implements CObject {
    private handle: number

    constructor(content?: string) {
        const handle = MtfmtWASM.mstr_wasm_new_string()
        if (handle === 0) {
            throw WebAssembly.RuntimeError('Cannot allocate memory for CString object')
        }
        // 初始化字符串
        if (content !== undefined) {
            this.handle = CString.cstr_cont(content, s => {
                const code = MtfmtWASM.mstr_create(handle, s)
                if (code !== MStrResult.MStr_Ok) {
                    throw WebAssembly.RuntimeError('Cannot create CString object')
                }
                return handle
            })
        } else {
            const code = MtfmtWASM.mstr_create(handle, 0)
            if (code !== MStrResult.MStr_Ok) {
                throw WebAssembly.RuntimeError('Cannot create CString object')
            }
            this.handle = handle
        }
    }

    /**
     * 往末尾添加字符串
     * 
     * @param str: 需要增加的字符串
     */
    public append(str: string) {
        const handle = this.handle
        CString.cstr_cont(str, s => {
            const code = MtfmtWASM.mstr_concat_cstr(handle, s)
            if (code !== MStrResult.MStr_Ok) {
                throw WebAssembly.RuntimeError('Cannot append to CString object')
            }
        })
    }

    /**
     * 清空字符串
     */
    public clear() {
        MtfmtWASM.mstr_clear(this.handle)
    }

    /**
     * 取得字符串内容
     * 
     * @returns 字符串
     */
    public get(): string {
        const ret_handle = this.handle
        // as cptr
        const c_rawstr = MtfmtWASM.mstr_as_cstr(ret_handle)
        // 取得字符串内容
        const buffer = MtfmtWASM.memory.buffer
        const length = MtfmtWASM.mstr_wasm_string_len(ret_handle)
        const rawstr = new Uint8Array(buffer, c_rawstr, length)
        // 取得字符串内容
        const decoder = new TextDecoder()
        const result = decoder.decode(rawstr)
        return result
    }

    public destroy() {
        const handle = this.handle
        MtfmtWASM.mstr_free(handle)
        MtfmtWASM.mstr_wasm_free_string(handle)
    }

    /**
    * 将字符串转为cstr处理
    * 
    * @param str: 字符串
    * @param cont: cont
    */
    private static cstr_cont<T>(str: string, cont: (str: number) => T): T {
        const encoder = new TextEncoder()
        const null_term = str + '\0'
        const encoded_string = encoder.encode(null_term)
        // 分配内存给字符串
        const pstr = MtfmtWASM.mstr_wasm_allocate(encoded_string.length)
        if (pstr === 0) {
            throw WebAssembly.RuntimeError('Cannot allocate memory for c string')
        }
        // 处理内容
        const dst = new Uint8Array(MtfmtWASM.memory.buffer, pstr)
        dst.set(encoded_string)
        const result = cont(pstr)
        return result
    }
}

/**
 * 保证obj能被正确销毁的情况下执行代码
 * 
 * @param obj: 对象
 * @param cont: cont
 */
export function pcall<T extends CObject, R>(obj: T, cont: (obj: T) => R): R {
    const r = cont(obj)
    obj.destroy()
    return r
}
