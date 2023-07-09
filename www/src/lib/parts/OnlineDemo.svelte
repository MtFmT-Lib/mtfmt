<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Caption from './Caption.svelte'
    import Monaco from '@comp/Monaco.svelte'
    import * as TryItText from '@text/try.txt?raw'
    import * as ExampleCode from '@text/example.c?raw'
</script>

<div>
    <Caption caption_text="Try it online" />
    <div class="tryit-hints">{@html TryItText.default}</div>
    <div class="demo">
        <div class="demo-block">
            <div class="editor">
                <Monaco default_code={ExampleCode.default} />
            </div>
            <div class="result">
                <div>TODO</div>
            </div>
        </div>
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars';

    .tryit-hints {
        font-family: $text-font-en;
        letter-spacing: 0.25px;
        line-height: 180%;
        margin-top: 0;
        margin-bottom: 2em;
    }

    .demo-block {
        gap: 0.5em;
        display: flex;
        flex-direction: row;
        align-items: stretch;
        flex-wrap: nowrap;

        @media screen and (width < $sm) {
            flex-direction: column;
        }
    }

    .editor {
        flex: 0 0 50%;
        box-sizing: border-box;
        background-color: var(--bg-color);
    }

    .result {
        flex: 0 0 50%;
        box-sizing: border-box;
        background-color: var(--bg-color);
        padding: 0.3em 0.5em;
        font-family: $mono-text-font;
        overflow: auto;
    }
</style>
