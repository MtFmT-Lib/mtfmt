<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<div class="nav-items">
    <a href="./">HOME</a>
    <a href="./start">QUICK START</a>
    <a href="./faq">FAQ</a>
    <a href="./doc">DOC</a>
    <a href="https://mtfmt-lib.github.io/mtfmt/doxygen/html/">CODE DOC</a>
    <a href="https://github.com/MtFmT-Lib/mtfmt">GITHUB</a>
</div>

<style lang="scss">
    @import '@theme/stylevars.scss';

    .nav-items a {
        font-size: inherit;
        white-space: nowrap;
        text-decoration: none;
        color: var(--hpyer-link-color);

        &:hover {
            color: var(--hpyer-link-color-hover);
        }

        &::before {
            content: '\00b7';
            padding-left: 0.5em;
            padding-right: 0.5em;
        }

        &:first-child:before {
            content: '';
            padding: 0;
        }
    }
</style>
