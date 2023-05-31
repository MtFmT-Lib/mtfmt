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

<div class="markdown-box">
    {@html generate_toc(html, toc)}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .markdown-box {
        margin-top: 1em;
        margin-bottom: 1em;
        line-height: 150%;
        font-size: 1.1rem;
    }
</style>
