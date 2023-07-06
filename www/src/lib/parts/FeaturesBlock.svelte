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

<div class="block-groups">
    {#each items as item}
        <div class="block-item">
            <div>
                <div class="block-header">{item[0]}</div>
                <div class="block-content">
                    {@html item[1].brief}
                </div>
            </div>
            <div class="block-link">
                {#if item[1].details.type === 'hyper'}
                    <a class="block-hyper-link" href={item[1].details.value}>
                        {item[1].details.text ?? item[1].details.value}
                    </a>
                {:else}
                    <!-- TODO -->
                {/if}
            </div>
        </div>
    {/each}
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .block-groups {
        display: grid;
        align-items: stretch;
        grid-auto-flow: row dense;
        grid-row-gap: 2em;
        grid-column-gap: 0.8em;
        grid-template-rows: 1fr auto;
        grid-template-columns: repeat(auto-fit, minmax(12em, 1fr));
    }

    .block-item {
        hyphens: auto;
        font-family: $text-font-en;
        font-variant-ligatures: contextual;
        display: flex;
        flex-direction: column;
        align-items: stretch;
        justify-content: space-between;

        @media screen and (width < $xs) {
            padding-left: 1em;
            border-left: 2px solid var(--border-color-deep);
        }
    }

    .block-header {
        font-size: 110%;
        letter-spacing: 0.5px;
        font-weight: 500;
        margin-bottom: 0.8em;
    }

    .block-link {
        margin-top: 1em;
    }

    .block-hyper-link {
        display: block;

        &::before {
            content: '>';
            margin-right: 0.3em;
        }
    }
</style>
