<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Caption from './Caption.svelte'
    import Monaco from '@comp/Monaco.svelte'
</script>

<div>
    <Caption caption_text="Try it online" />
    <div class="demo-block">
        <div class="editor">
            <Monaco />
        </div>
        <div class="result">Result</div>
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars';

    .demo-block {
        display: flex;
        flex-direction: row;
        align-items: stretch;
        flex-wrap: nowrap;
    }

    .editor {
        flex: 0 0 60%;
        box-sizing: border-box;
    }

    .result {
        flex: 0 0 40%;
        box-sizing: border-box;
    }

    @media screen and (width < $sm) {
        .demo-block {
            flex-direction: column;
        }
    }
</style>
