<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import NavBar from '@comp/NavBar.svelte'
    import TransText from '@comp/TransText.svelte'
</script>

<div class="container">
    <div class="main-box">
        <div class="main-content">
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

    .main-box {
        width: 100%;
        height: 100%;
        overflow-y: hidden;

        // 内边距
        padding: 0.5em 0.5em;
        box-sizing: border-box;
    }

    .main-box {
        text-align: center;
        display: flex;
        align-items: center;
        justify-content: center;
    }

    .main-content {
        border: 1px solid $border-color;
        border-radius: 4px;
        padding: 2em 2em;
        box-sizing: border-box;

        // 外边距
        margin-bottom: 4em;
    }

    .title {
        font-size: 2.25em;
        font-weight: 600;
        margin-bottom: 0.5em;
    }

    .brief {
        font-size: 1.15em;
        margin-bottom: 1.15em;
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
