<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Header from '@part/Header.svelte'
    import Footer from '@part/Footer.svelte'
    import Caption from '@part/Caption.svelte'
    import TransText from '@part/TransText.svelte'
    import OnlineDemo from '@part/OnlineDemo.svelte'
    import SeealsoJSON from '@text/seealso.json'
    import FeaturesJSON from '@text/features.json'
    import { record_to_map } from '$lib/fp/cast'
    import * as BriefText from '@text/brief.txt?raw'
    import FeaturesBlock from '@part/FeaturesBlock.svelte'
    import InstallSelector from '@part/InstallSelector.svelte'
    import type { ItemContent } from '@part/features_block'

    /**
     * features
     */
    const feature_item = record_to_map<string, ItemContent>(
        FeaturesJSON,
        (s) => s
    )

    /**
     * see also
     */
    const seealso_item = record_to_map<string, ItemContent>(
        SeealsoJSON,
        (s) => s
    )
</script>

<div class="container">
    <Header />
    <div class="top">
        <div class="top-inner">
            <div class="top-logo">
                <TransText text="MtFmt" />
            </div>
            <div class="top-brief">
                {@html BriefText.default}
            </div>
        </div>
    </div>
    <div class="content-box-odd">
        <div class="contents">
            <Caption caption_text="features" />
            <FeaturesBlock items={feature_item} />
        </div>
    </div>
    <div class="content-box">
        <div class="contents">
            <OnlineDemo />
        </div>
    </div>
    <div class="content-box-odd">
        <div class="contents">
            <Caption caption_text="see also" />
            <FeaturesBlock items={seealso_item} />
        </div>
    </div>
    <Footer />
</div>

<svelte:head>
    <title>Mtfmt library</title>
    <meta name="description" content="MtFmt library" />
</svelte:head>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .container {
        position: absolute;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;

        // 祈愿可以让width, height按照屏幕大小计算
        // 并且允许内容溢出
        overflow: visible;
    }

    .contents {
        width: 80%;
        max-width: $content-text-max-width;
        margin: 0 auto;
    }

    .top {
        height: 80%;
        display: flex;
        margin: 0 auto;
        align-items: center;
        justify-content: center;

        @media screen and (width < $sm) {
            height: 40%;
        }
    }

    .top-inner {
        text-align: center;
    }

    .top-logo {
        font-size: 250%;
        letter-spacing: 0.5px;
        font-weight: var(--caption-text-weight);

        @media screen and (width < $sm) {
            font-size: 225%;
        }
    }

    .top-brief {
        font-size: 150%;

        @media screen and (width < $sm) {
            font-size: 125%;
        }
    }

    .content-box,
    .content-box-odd {
        width: 100%;
        min-height: 30%;
        padding-top: 1em;
        padding-bottom: 2em;
    }

    .content-box {
        background-color: var(--even-content-bg-color);
    }

    .content-box-odd {
        background-color: var(--odd-content-bg-color);
        border-top: 1px solid var(--border-color);
        border-bottom: 1px solid var(--border-color);
    }

    @media screen and (width < 720px) {
        .contents {
            width: 95%;
            max-width: initial;
        }
    }
</style>
