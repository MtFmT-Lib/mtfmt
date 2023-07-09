<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@part/Header.svelte'
    import Footer from '@part/Footer.svelte'
    import Markdown from '@comp/Markdown.svelte'
    import * as EnPage from '@text/en/QUICKSTART.md'
    import * as ZhPage from '@text/zh/QUICKSTART.md'

    const contents = {
        en: { toc: EnPage.toc, html: EnPage.html },
        zh: { toc: ZhPage.toc, html: ZhPage.html },
    }
</script>

<svelte:head>
    <title>Mtfmt library - Quick start</title>
    <meta name="description" content="MtFmt library quick start" />
</svelte:head>

<Header />
<Markdown {contents} />
<Footer />
