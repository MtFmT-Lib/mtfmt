<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import TransText from '@part/TransText.svelte'

    /**
     * 年份
     */
    const year = new Date().getFullYear().toString()
</script>

<footer class="footer-box">
    <div class="contents">
        <div class="name">
            <TransText text="MtFmt" />
        </div>
        <div>
            <div class="section">document</div>
            <div class="footer-sub-content">
                <ul class="supports-links">
                    <li><a href="./start">QUICK START</a></li>
                    <li><a href="./doc">API DOCUMENT</a></li>
                    <li>
                        <a
                            href="https://mtfmt-lib.github.io/mtfmt/doxygen/html/"
                        >
                            <span>SOURCES DOCUMENT</span>
                        </a>
                    </li>
                </ul>
            </div>
        </div>
        <div>
            <div class="section">sources</div>
            <div class="footer-sub-content">
                <a href="https://github.com/MtFmT-Lib/mtfmt">GITHUB</a>
            </div>
        </div>
        <div class="copyright">
            <div class="footer-sub-content">
                &copy; {year},
                <a
                    href="https://github.com/MtFmT-Lib/mtfmt/graphs/contributors"
                >
                    <span>MtFmt contributors</span>
                </a>
            </div>
        </div>
        <div class="brief">
            <div class="footer-sub-content">
                MtFmt is a modern string formatting library wrote by pure C
            </div>
        </div>
    </div>
</footer>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .footer-box {
        width: 100%;
        padding-top: 3em;
        padding-bottom: 3em;
        background-color: var(--footer-bg-color);
    }

    .contents {
        width: 80%;
        max-width: $footer-max-width;
        margin: 0 auto;
        display: grid;
        justify-items: center;
        align-items: baseline;
        grid-row-gap: 6rem;
        grid-template-rows: 1fr;
        grid-template-columns: 1fr 1fr 1fr;
    }

    .section {
        font-size: 120%;
        font-weight: 500;
        letter-spacing: 0.5px;
        text-transform: uppercase;
    }

    .name {
        font-size: 150%;
        font-weight: 600;
        letter-spacing: 0.5px;
    }

    .footer-sub-content {
        margin-top: 1em;
        color: var(--footer-content-color);
    }

    .supports-links {
        margin: 0;
        padding: 0;
        list-style-type: none;

        li {
            padding-top: 0.25em;
            padding-bottom: 0.25em;
        }
    }

    .brief {
        grid-column: span 2;
    }

    a {
        text-decoration: none;

        &:hover {
            color: var(--text-color);
        }
    }

    @media screen and (width < 720px) {
        .contents {
            width: 95%;
            max-width: initial;
        }
    }
</style>
