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
            <!-- ignored. -->
        </div>
    </div>
</footer>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .footer-box {
        width: 100%;
        padding: 1em 1em;
        box-sizing: border-box;
        background-color: var(--footer-bg-color);
        border-top: 1px solid var(--border-color);
    }

    .contents {
        width: 80%;
        max-width: $footer-max-width;
        margin: 0 auto;
        display: grid;
        justify-items: center;
        align-items: baseline;
        grid-row-gap: 1em;
        grid-column-gap: 0.25em;
        grid-template-rows: 1fr 1fr;
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

    @media screen and (width < 1080px) {
        .contents {
            width: 100%;
        }

        .copyright {
            grid-column: span 2;
            justify-self: start;
        }

        .brief {
            grid-column: span 1;
            display: none;
        }
    }
</style>
