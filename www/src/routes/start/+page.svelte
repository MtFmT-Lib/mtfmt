<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@comp/Header.svelte'
    import Markdown from '@comp/Markdown.svelte'
    import { toc, html } from '@text/QUICKSTART.md'
</script>

<svelte:head>
    <title>Mtfmt library - Quick start</title>
    <meta name="description" content="MtFmt library quick start" />
</svelte:head>

<div>
    <Header />
    <Markdown {html} {toc} />
</div>
