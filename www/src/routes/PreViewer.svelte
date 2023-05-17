<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import { test } from "../mtfmt/lib";

    function eval_test() {
        test();
    }
</script>

<div>
    <button on:click={() => eval_test()}>114514</button>
</div>
