// SPDX-License-Identifier: LGPL-3.0

/**
 * C对象接口
 */
export interface CObject {
    destroy(): void
}

/**
 * 可释放的对象
 */
export default class ScopeObject<T extends CObject>{
    private obj: T

    constructor(obj: { new(): T }) {
        this.obj = new obj()
    }

    public with<R>(then: (obj: T) => R): R {
        const result = then(this.obj)
        this.obj.destroy()
        return result
    }
}
