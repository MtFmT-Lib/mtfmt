// SPDX-License-Identifier: LGPL-3.0
import MtfmtWASM from './mtfmt'

export class MtFmt {

    /**
     * 取得库的wrapper版本号
     * 
     * @returns 版本号
     */
    public static wrap_version(): number {
        return MtfmtWASM.mstr_wasm_version()
    }
}

export function test() {
    console.log(MtfmtWASM)
    console.log(MtFmt.wrap_version())
}
