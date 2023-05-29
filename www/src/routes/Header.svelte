<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<div>
    <div class="header">
        <h1 class="title">
            <span class="odd">M</span><span class="even">t</span><span
                class="mid">F</span
            ><span class="even">m</span><span class="odd">t</span>
        </h1>
        <div class="brief">
            A clean and efficient pure c language library for string formatting.
        </div>
    </div>
    <div class="seealso">
        <a href="/">HOME</a>
        <a href="/start">QUISK START</a>
        <a href="/doc">DOCUMENTS</a>
        <a href="https://mtfmt-lib.github.io/mtfmt/doc/html">CODE DOCUMENTS</a>
        <a href="https://github.com/MtFmT-Lib/mtfmt">GITHUB</a>
    </div>
</div>

<style lang="scss">
    @import "./stylevars.scss";

    .header {
        margin-bottom: 1em;
        padding-bottom: 1em;
        width: 100%;
        border: none;
        border-bottom: 1px solid $border-color;

        h1.title {
            padding: 0;
            margin: 0;
            font-size: 2em;
            padding-top: 0.25em;
            padding-bottom: 0.25em;
            margin-top: 0.25em;
            margin-bottom: 0.25em;

            .odd {
                color: $transflag-blue;
            }

            .even {
                color: $transflag-pink;
            }

            .mid {
                color: $transflag-gray;
            }
        }

        .brief {
            font-size: 0.9em;
        }
    }

    .seealso {
        margin-top: 0.5em;
        margin-bottom: 1em;

        a {
            text-decoration: none;
            color: $hpyer-link-color;

            &:hover {
                color: mix($hpyer-link-color, $bg-color, 50%);
            }

            &::before {
                content: "\00b7";
                padding-left: 0.5em;
                padding-right: 0.5em;
            }

            &:first-child {
                &::before {
                    content: "";
                    padding: 0;
                }
            }
        }
    }
</style>
