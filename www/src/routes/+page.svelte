<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@part/Header.svelte'
    import Footer from '@part/Footer.svelte'
    import Caption from '@part/Caption.svelte'
    import OnlineDemo from '@part/OnlineDemo.svelte'
    import InstallSelector from '@part/InstallSelector.svelte'
</script>

<div class="container">
    <Header />
    <div class="top">TODO</div>
    <div class="content-box-odd">
        <div class="contents">
            <InstallSelector />
        </div>
    </div>
    <div class="content-box">
        <div class="contents">
            <OnlineDemo />
        </div>
    </div>
    <div class="content-box-odd">
        <div class="contents">
            <Caption caption_text="contributors" />
        </div>
    </div>
    <div class="content-box">
        <div class="contents">
            <Caption caption_text="see also" />
        </div>
    </div>
    <Footer />
</div>

<svelte:head>
    <title>Mtfmt library</title>
    <meta name="description" content="MtFmt library" />
</svelte:head>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .container {
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
    }

    .contents {
        width: 80%;
        max-width: $header-max-width;
        margin: 0 auto;
    }

    .top {
        width: 100%;
        height: 80%;
        display: flex;
        align-items: center;
        justify-content: center;
    }

    .content-box,
    .content-box-odd {
        width: 100%;
        height: 50%;
        padding-top: 1em;
        padding-bottom: 2em;
    }

    .content-box {
        background-color: var(--even-content-bg-color);
    }

    .content-box-odd {
        background-color: var(--odd-content-bg-color);
        border-top: 1px solid var(--border-color);
        border-bottom: 1px solid var(--border-color);
    }

    @media screen and (width < 720px) {
        .contents {
            width: 95%;
            max-width: initial;
        }
    }
</style>
