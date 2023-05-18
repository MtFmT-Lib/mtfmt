// SPDX-License-Identifier: LGPL-3.0

export declare function mstr_create(pstr: number, content: number): MStrResult

export declare function mstr_as_cstr(pstr: number): number

export declare function mstr_clear(pstr: number)

export declare function mstr_concat_cstr(pstr: number, cstr: number): MStrResult

export declare function mstr_free(pstr: number)

export declare function mstr_wasm_version(): number

export declare function mstr_wasm_new_string(): number

export declare function mstr_wasm_string_len(ptr: number): number

export declare function mstr_wasm_free_string(ptr: number)

export declare function mstr_wasm_allocate(size: number): number

export declare function mstr_wasm_free(ptr: number)

export declare const memory: WebAssembly.Memory 
