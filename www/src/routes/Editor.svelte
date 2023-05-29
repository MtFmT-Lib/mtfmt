<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    let tests = [
        {
            type: "i32",
        },
        {
            type: "i16",
        },
    ];
</script>

<div class="structure-editor">
    <div class="string-input">
        <input type="text" placeholder="Format string" />
    </div>
    <div class="arg-inputs">
        <div class="arg-global-hint">with</div>
        <div class="arg-input">
            {#each tests as item, i}
                <div class="arg-input-item">
                    <div class="arg-input-item-argid">{i}=</div>
                    <input
                        class="arg-input-item-argval"
                        type="text"
                        placeholder="Argument {i}:{item.type}"
                    />
                </div>
            {/each}
        </div>
    </div>
</div>

<style lang="scss">
    @import "./stylevars.scss";

    .structure-editor {
        input[type="text"] {
            margin-left: 0.25em;
            outline: none;
            border: none;
            border-bottom: 2px solid $border-color;
        }
    }

    .string-input {
        width: 100%;
    }

    .arg-inputs {
        display: flex;
        flex-direction: row;
        align-items: flex-start;
        justify-content: flex-start;
        margin-top: 0.5em;
    }

    .arg-input-item {
        margin-left: 0.4em;
        margin-bottom: 0.4em;
        display: flex;
        align-items: baseline;
        justify-content: flex-start;
        flex-direction: row;
    }
</style>
