<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    /**
     * 要显示的标题文本
     */
    export let caption_text: string
</script>

<div class="caption">{caption_text}</div>

<style lang="scss">
    .caption {
        margin-top: 1em;
        margin-bottom: 1em;
        font-size: 150%;
        font-weight: var(--caption-text-weight);
        font-family: var(--caption-font);
        letter-spacing: var(--text-caption-space);
        text-transform: uppercase;
    }
</style>
