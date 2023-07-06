<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import type { Items } from './features_block'

    /**
     * 项
     */
    export let items: Items = new Map()
</script>

<div>
    {#each items as item}
        {item[0]}
    {/each}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';
</style>
