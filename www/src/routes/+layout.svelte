<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script>
    import "./style.scss";
</script>

<div class="outter">
    <div class="app">
        <main>
            <slot />
        </main>
    </div>
</div>

<style lang="scss">
    @import "./stylevars.scss";

    .outter {
        width: 100%;
        height: 100%;
    }

    .app {
        border-radius: 2px;
        border: 1px solid $border-color;
        max-width: 720px;
        width: 80%;
        min-width: 360px;
        margin: 0 auto;
        margin-top: 3em;
        margin-bottom: 3em;
        padding: 1em 1em;
    }
</style>
