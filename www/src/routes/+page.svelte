<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Viewer from "./Viewer.svelte";
    import Header from "./Header.svelte";
</script>

<svelte:head>
    <title>Mtfmt library</title>
    <meta name="description" content="MtFmt library" />
</svelte:head>

<section>
    <Header />
    <div class="active">
        <Viewer />
    </div>
</section>

<style lang="scss">
    @import "./stylevars.scss";

    .active {
        margin-top: 2em;
        margin-bottom: 1.5em;
    }
</style>
