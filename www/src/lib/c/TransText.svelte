<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    /**
     * 文本内容
     */
    export let text: string
</script>

<div class="text">
    {#each text as content_char, index}
        {#if index === Math.floor(text.length / 2)}
            <span class="mid">{content_char}</span>
        {:else if index % 2 === 0}
            <span class="odd">{content_char}</span>
        {:else}
            <span class="even">{content_char}</span>
        {/if}
    {/each}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .text {
        .odd {
            color: $transflag-blue;
        }

        .even {
            color: $transflag-pink;
        }

        .mid {
            color: $transflag-gray;
        }
    }
</style>
