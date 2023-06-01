<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import generate_toc from '$lib/tocmake'

    /**
     * HTML内容
     */
    export let html: string

    /**
     * 目录内容
     */
    export let toc: { level: string; content: string }[]
</script>

<div class="markdown">
    <div class="markdown-toolbar">
        <div class="reader-tools">
            <button>BIO</button>
        </div>
        <div class="reader-language">
            <button>EN</button>
        </div>
    </div>
    <div class="markdown-box">
        {@html generate_toc(html, toc)}
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .markdown {
        padding-bottom: 0.5em;
    }

    .markdown-toolbar {
        display: flex;
        justify-content: space-between;
        align-items: baseline;
        flex-direction: row;
        color: mix($bg-color, $text-color, 40%);

        // 颜色和padding
        padding-top: 0.25em;
        padding-bottom: 0.5em;

        // 边框
        border-top: 1px solid $border-color;
    }

    .reader-tools button,
    .reader-language button {
        outline: 0;
        border: none;
        color: $hpyer-link-color;
        background-color: $bg-color;
        padding: 0.25em 0.25em;

        &:hover {
            background-color: mix($text-color, $bg-color, 15%);
        }

        &:active {
            background-color: mix($text-color, $bg-color, 40%);
        }
    }

    .markdown-box {
        line-height: 150%;
        font-size: 1.1rem;
    }
</style>
