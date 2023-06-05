// SPDX-License-Identifier: LGPL-3.0

enum ResultTag {
    Ok = 0, Err = 1
}

export default class Result<T, E>{
    private value: T | E

    private type_tag: ResultTag

    private constructor(value: T | E, type_tag: ResultTag) {
        this.value = value
        this.type_tag = type_tag
    }

    public and_then<R>(then_do: (value: T) => Result<R, E>): Result<R, E> {
        if (this.type_tag === ResultTag.Err) {
            return Result.Err<R, E>(this.value as E)
        } else {
            return then_do(this.value as T)
        }
    }

    public or(or: T): T {
        if (this.type_tag === ResultTag.Ok) {
            return this.value as T
        } else {
            return or
        }
    }

    public or_map(map: (value: E) => T): T {
        if (this.type_tag === ResultTag.Ok) {
            return this.value as T
        } else {
            return map(this.value as E)
        }
    }

    public map<R>(map: (value: T) => R): Result<R, E> {
        if (this.type_tag === ResultTag.Ok) {
            return Result.Ok<R, E>(map(this.value as T))
        } else {
            return Result.Err<R, E>(this.value as E)
        }
    }

    public static Ok<T, E>(value: T): Result<T, E> {
        return new Result<T, E>(value, ResultTag.Ok)
    }

    public static Err<T, E>(value: E): Result<T, E> {
        return new Result<T, E>(value, ResultTag.Err)
    }
}

/**
 * Optional
 */
export type Optional<T> = Result<T, void>

/**
 * 表示某些值
 */
export function Some<T>(value: T): Optional<T> {
    return Result.Ok<T, void>(value)
}

/**
 * 表示没有值
 */
export function None<T>(): Optional<T> {
    return Result.Err<T, void>(undefined)
}
