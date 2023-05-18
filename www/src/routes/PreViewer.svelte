<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    function eval_test() {}
</script>

<div class="demo">
    <div class="left-part">
        <button on:click={() => eval_test()}>114514</button>
    </div>
    <div class="center-part">&gt;=&lt;</div>
    <div class="right-part">114514</div>
</div>

<style lang="scss">
    @import "./stylevars.scss";

    .demo {
        display: flex;
        flex-direction: row;
        justify-content: space-between;
        align-items: stretch;
    }

    .left-part,
    .right-part {
        flex-grow: 1;
    }

    .left-part {
        padding-right: 1em;
    }
    .center-part {
        text-align: center;
    }

    .right-part {
        padding-left: 1em;
    }
</style>
