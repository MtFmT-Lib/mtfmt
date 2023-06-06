<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import NavBar from './NavBar.svelte'
    import TransText from './TransText.svelte'
</script>

<div class="header-father">
    <div class="header">
        <h1 class="title">
            <TransText text="MtFmt" />
        </h1>
        <div class="brief">
            A clean and efficient pure c language library for string formatting.
        </div>
    </div>
    <div class="seealso">
        <NavBar />
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .header-father {
        display: flex;
        flex-direction: column;
        flex-wrap: nowrap;
        align-items: stretch;
    }

    .header {
        width: 100%;
        padding-bottom: 1em;

        .title {
            padding: 0;
            margin: 0;
            font-size: 2em;
            padding-top: 0.25em;
            padding-bottom: 0.25em;
            margin-top: 0.25em;
            margin-bottom: 0.25em;
        }

        .brief {
            font-size: 0.9em;
        }
    }

    .seealso {
        margin-top: 0.5em;
        margin-bottom: 1em;
    }

    @media screen and (max-width: 640px) {
        .header .brief {
            font-size: 0.85em;
        }

        .seealso {
            font-size: 0.85em;
        }
    }
</style>
