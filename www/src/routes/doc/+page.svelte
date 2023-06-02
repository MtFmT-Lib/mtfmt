<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@comp/Header.svelte'
    import Markdown from '@comp/Markdown.svelte'
    import * as EnPage from '@text/en/DOCUMENT.md'

    const contents = {
        en: { toc: EnPage.toc, html: EnPage.html },
    }

    const language = Object.keys(contents)
</script>

<svelte:head>
    <title>Mtfmt library - Document</title>
    <meta name="description" content="MtFmt library document" />
</svelte:head>

<div>
    <Header />
    <Markdown {language} {contents} />
</div>
