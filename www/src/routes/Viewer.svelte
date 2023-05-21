<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Editor from "./Editor.svelte";
    import { CString } from "$lib/mtfmt/lib";
    import ScopeObject from "$lib/mtfmt/scope";
    import IntoImage from "$lib/images/into.svg";
    import FromImage from "$lib/images/from.svg";

    export const prerender = true;

    function into_action() {
        // TODO
    }

    function from_action() {
        // TODO
    }
</script>

<div class="demo">
    <div class="left-part">
        <Editor />
    </div>
    <div class="center-part">
        <button on:click={into_action}>
            <img src={IntoImage} alt="Into" />
        </button>
        <br />
        <button on:click={from_action}>
            <img src={FromImage} alt="Scan" />
        </button>
    </div>
    <div class="right-part">
        <textarea placeholder="Here is the output." />
    </div>
</div>

<style lang="scss">
    @import "./stylevars.scss";

    .demo {
        display: flex;
        flex-direction: row;
        justify-content: space-between;
        align-items: stretch;
        min-height: 8em;
    }

    .left-part,
    .right-part {
        flex-grow: 1;
    }

    .left-part {
        border-right: 1px solid $border-color;
    }

    .center-part {
        flex-grow: 0;
        margin-left: 1em;
        margin-right: 1em;
        text-align: center;
        vertical-align: middle;
        line-height: 4em;

        button {
            height: 4em;
            background-color: $bg-color;
            outline: none;
            border: none;

            &:hover {
                background-color: $border-color;
            }

            &:active {
                background-color: mix($border-color, black, 40%);
            }

            img {
                width: 4em;
            }
        }
    }

    .right-part textarea {
        border: none;
        border-left: 1px solid $border-color;
        resize: none;
        font-size: inherit;
        font-family: inherit;
        outline: none;
        width: 100%;
        height: 100%;
    }
</style>
