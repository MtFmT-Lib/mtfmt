// SPDX-License-Identifier: LGPL-3.0
export declare type MString = object

export declare function mstr_create(pstr: number, content: number): number

export declare function mstr_as_cstr(pstr: number): string

export declare function mstr_append(pstr: number, ch: number): number

export declare function mstr_free(pstr: number)

export declare function mstr_wasm_version(): number

export declare function mstr_wasm_new_string(): number

export declare function mstr_wasm_string_len(ptr: number): number

export declare function mstr_wasm_free_string(ptr: number)
