<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from "@comp/Header.svelte";
    import Markdown from "@comp/Markdown.svelte";
    import { toc, html } from "@text/FAQ.md";
</script>

<svelte:head>
    <title>Mtfmt library - FAQ</title>
    <meta name="description" content="MtFmt library faq" />
</svelte:head>

<div>
    <Header />
    <Markdown html={html} toc={toc} />
</div>
