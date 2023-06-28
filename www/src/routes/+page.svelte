<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@comp/Header.svelte'
    import Footer from '@comp/Footer.svelte'
    import NavBar from '@comp/NavBar.svelte'
    import TransText from '@comp/TransText.svelte'
    import OnlineDemo from '@comp/OnlineDemo.svelte'
    import InstallSelector from '@comp/InstallSelector.svelte'
</script>

<div class="container">
    <div class="top">
        <div class="top-inner">
            <div class="title">
                <TransText text="MtFmt" />
            </div>
            <div class="brief">
                A clean and efficient pure c language library for string
                formatting.
            </div>
            <div class="nav">
                <NavBar />
            </div>
        </div>
    </div>
    <div class="content-box-odd">
        <div class="content">
            <InstallSelector />
        </div>
    </div>
    <div class="content-box">
        <div class="content">
            <OnlineDemo />
        </div>
    </div>
    <Footer />
</div>

<svelte:head>
    <title>Mtfmt library</title>
    <meta name="description" content="MtFmt library" />
</svelte:head>

<style lang="scss">
    .container {
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
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
        height: 40%;
    }

    .content-box {
        background-color: var(--even-content-bg-color);
    }

    .content-box-odd {
        background-color: var(--odd-content-bg-color);
        border-top: 1px solid var(--border-color);
        border-bottom: 1px solid var(--border-color);
    }

    .title {
        font-size: 2.25em;
        font-weight: 600;
        margin-bottom: 0.5em;
        text-align: center;
    }

    .brief {
        font-size: 1.15em;
        margin-bottom: 1.15em;
        text-align: center;
    }

    .nav {
        text-align: center;
    }

    @media screen and (max-width: 640px) {
        .brief {
            font-size: 0.85em;
        }

        .title {
            font-size: 1.5em;
        }

        .nav {
            font-size: 0.85em;
        }
    }
</style>
