<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import TransText from '@part/TransText.svelte'
</script>

<div class="header-father">
    <div class="header-content">
        <div class="title">
            <a href="./"><TransText text="MtFmt" /></a>
        </div>
        <div class="nav-items">
            <a href="./">HOME</a>
            <a href="./start">QUICK START</a>
            <a href="./faq">FAQ</a>
            <a href="./doc">DOC</a>
            <a href="https://mtfmt-lib.github.io/mtfmt/doxygen/html/">
                <span>CODE DOC</span>
            </a>
            <a href="https://github.com/MtFmT-Lib/mtfmt">GITHUB</a>
        </div>
    </div>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .header-father {
        width: 100%;
        display: flex;
        align-items: center;
        justify-content: center;
    }

    .header-content {
        width: 80%;
        max-width: $header-max-width;
        height: 8vh;
        display: flex;
        align-items: center;
        justify-content: space-between;
        margin: 0 auto;
    }

    .title {
        font-size: 180%;
        font-weight: 600;
        letter-spacing: 0.5px;
        box-sizing: border-box;
        padding-left: 1em;

        a {
            color: inherit;
            text-decoration: none;
        }
    }

    .nav-items {
        padding-right: 0.8em;

        a {
            font-size: inherit;
            white-space: nowrap;
            text-decoration: none;
            padding-right: 1em;
            color: var(--hpyer-link-color);

            &:hover {
                color: var(--hpyer-link-color-hover);
            }

            &:last-child {
                padding: 0;
            }
        }
    }

    @media screen and (width < 1080px) {
        .header-content {
            width: 100%;
        }
    }
</style>
