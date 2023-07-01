<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { page } from '$app/stores'
    import { writable } from 'svelte/store'
    import TransText from '@part/TransText.svelte'

    /**
     * 当前显示的page的id
     */
    let cur_page_id = writable('/')

    page.subscribe((cur_page) => {
        cur_page_id.set(cur_page.route.id ?? '/')
    })

    /**
     * 链接
     */
    const links: { addr: string; name: string }[] = [
        { addr: '/', name: 'HOME' },
        { addr: '/start', name: 'QUICK START' },
        { addr: '/faq', name: 'FAQ' },
        { addr: '/doc', name: 'DOC' },
        {
            addr: 'https://mtfmt-lib.github.io/mtfmt/doxygen/html/',
            name: 'CODE DOC',
        },
        { addr: 'https://github.com/MtFmT-Lib/mtfmt', name: 'GITHUB' },
    ]
</script>

<div class="header-father">
    <div class="header-content">
        <div class="title">
            <a href="./"><TransText text="MtFmt" /></a>
        </div>
        <ul class="nav-items">
            {#each links as link}
                {#if link.addr === $cur_page_id && link.addr !== '/'}
                    <li class="nav-actived">
                        <a href={link.addr}>{link.name}</a>
                    </li>
                {:else}
                    <li class="nav-inactived">
                        <a href={link.addr}>{link.name}</a>
                    </li>
                {/if}
            {/each}
        </ul>
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

        @media screen and (width < $lg) {
            width: 100%;
        }

        @media screen and (width < $md) {
            height: 6vh;
        }
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

        @media screen and (width < $md) {
            padding-left: 0.6em;
            font-size: 150%;
        }
    }

    .nav-items {
        margin: 0;
        padding: 0;
        padding-right: 0.8em;
        list-style-type: none;
        height: 100%;
        display: flex;
        align-items: stretch;

        li {
            display: flex;
            align-items: center;
            justify-items: center;
            padding-left: 0.5em;
            padding-right: 0.5em;

            &:last-child {
                padding-right: 0;
            }
        }

        a {
            font-size: inherit;
            white-space: nowrap;
            text-decoration: none;
            color: var(--hpyer-link-color);

            &:hover {
                color: var(--hpyer-link-color-hover);
            }
        }

        @media screen and (width < $md) {
            font-size: 0.9em;

            li {
                padding-right: 0.6em;
            }
        }
    }

    .nav-actived {
        border-bottom: 2px solid var(--border-color);
    }

    .nav-inactived {
        border-bottom: 2px solid transparent;
    }
</style>
