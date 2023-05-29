<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script>
    import "@style/style.scss";
</script>

<div class="outter">
    <div class="app">
        <main>
            <slot />
        </main>
    </div>
</div>

<style lang="scss">
    @import "@theme/stylevars.scss";

    .outter {
        width: 100%;
        height: 100%;
    }

    .app {
        border-radius: 2px;
        border: 1px solid $border-color;
        max-width: 800px;
        width: 80%;
        margin: 0 auto;
        margin-top: 3em;
        margin-bottom: 3em;
        padding: 1em 1em;
    }

    @media screen and (max-width: 720px) {
        .app {
            width: 90%;
        }
    }

    @media screen and (max-width: 640px) {
        .app {
            width: initial;
            border-left: none;
            border-right: none;
            padding: 0;
            margin-left: 1em;
            margin-right: 1em;
        }
    }
</style>
