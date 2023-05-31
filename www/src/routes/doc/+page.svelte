<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@comp/Header.svelte'
    import Markdown from '@comp/Markdown.svelte'
    import { toc, html } from '@text/DOCUMENT.md'
</script>

<svelte:head>
    <title>Mtfmt library - Document</title>
    <meta name="description" content="MtFmt library document" />
</svelte:head>

<div>
    <Header />
    <Markdown {html} {toc} />
</div>
