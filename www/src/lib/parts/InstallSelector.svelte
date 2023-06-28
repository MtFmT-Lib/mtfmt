<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Caption from './Caption.svelte'
    import * as InstallText from '@text/install.md'

    /**
     * 步骤
     */
    const steps = [
        {
            name: 'Prebuild',
            default: 'Sources',
            selector: 'Dymatic Library|Static Library|Sources',
        },
        {
            name: 'Plaftorm',
            default: 'NO-OS',
            selector: 'Windows|Linux|NO-OS',
        },
        {
            name: 'ISA',
            selector: 'amd64|x86|ARM v6m|ARM v7m|ARM v8m|any',
        },
        {
            name: 'Outputs',
            selector: 'CMSIS-PACK|Zip pack',
        },
    ]
</script>

<div>
    <Caption caption_text="Download MtFmt" />
    <div class="install-hints">
        {@html InstallText.html}
    </div>
    <ul class="steps">
        {#each steps as step}
            <li>
                <div class="step-name">{step.name}</div>
                <div class="step-selector">{step.selector}</div>
            </li>
        {/each}
        <li>
            <div class="finally-step-name">Download</div>
            <div class="step-selector">TODO</div>
        </li>
    </ul>
</div>

<style lang="scss">
    @import '@theme/stylevars';

    .install-hints {
        font-family: $text-font-en;
        letter-spacing: 0.25px;
        line-height: 180%;
    }

    .steps {
        padding: 0;
        margin: 0;
        margin-top: 1.5em;
        margin-bottom: 1em;
        list-style-type: none;

        li {
            display: flex;
            flex-wrap: nowrap;
            flex-direction: row;
            align-items: stretch;
            justify-content: flex-start;
            margin-top: 0.5em;
            margin-bottom: 0.5em;
        }
    }

    .finally-step-name,
    .step-name {
        padding: 0.5em 1.5em;
        flex-basis: 6vw;
    }

    .step-name {
        border-left: 2px solid var(--border-color-middle);
    }

    .finally-step-name {
        border-left: 2px solid var(--border-color-deep);
    }
</style>
