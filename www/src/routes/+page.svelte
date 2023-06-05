<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Viewer from '@comp/Viewer.svelte'
    import Header from '@comp/Header.svelte'
    import Markdown from '@comp/Markdown.svelte'
    import * as EnPage from '@text/en/HOMEPAGE.md'

    const contents = {
        en: { toc: EnPage.toc, html: EnPage.html },
    }

    const language = Object.keys(contents)
</script>

<svelte:head>
    <title>Mtfmt library</title>
    <meta name="description" content="MtFmt library" />
</svelte:head>

<section>
    <Header />
    <Markdown {language} {contents} />
    <div class="active">
        <Viewer />
    </div>
</section>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .active {
        margin-top: 2em;
        margin-bottom: 1.5em;
    }
</style>
