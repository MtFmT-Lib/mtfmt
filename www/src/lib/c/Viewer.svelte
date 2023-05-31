<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import Editor from './Editor.svelte'
    import { writable } from 'svelte/store'
    import ArrowIcon from '$lib/images/arrow.svg'

    /**
     * 是否从formatter变成scanner
     */
    let rev_direction = writable(false)

    /**
     * 更改方向
     */
    function change_direction() {
        rev_direction.set(!$rev_direction)
    }
</script>

<div class="demo">
    <div class="left-part">
        <Editor />
    </div>
    <div class="center-part">
        <button on:click={change_direction}>
            <img alt="Change direction" src={ArrowIcon} />
        </button>
    </div>
    <div class="right-part">
        <textarea placeholder="Output." value={$rev_direction.toString()} />
    </div>
</div>

<style lang="scss">
    @import '../../themes/common/stylevars.scss';

    .demo {
        display: flex;
        flex-direction: row;
        justify-content: space-between;
        align-items: center;
        min-height: 8em;
    }

    .left-part,
    .right-part {
        flex-grow: 1;
    }

    .center-part {
        flex-grow: 0;
        margin-left: 1em;
        margin-right: 1em;
        text-align: center;

        button {
            outline: none;
            border: none;
            overflow: hidden;
            background-color: transparent;

            img {
                height: 4em;
            }
        }
    }

    .right-part textarea {
        border: none;
        resize: none;
        font-size: inherit;
        font-family: inherit;
        outline: none;
        width: 100%;
        height: 100%;
    }
</style>
