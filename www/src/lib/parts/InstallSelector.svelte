<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import * as API from '$lib/github_api'
    import Caption from './Caption.svelte'
    import { curry_2 } from '$lib/fp/fun'
    import Button from '@comp/Button.svelte'
    import { apply_svelte } from '$lib/fp/apply'
    import * as InstallText from '@text/install.txt?raw'
    import RadioButtonGroup from '@comp/RadioButtonGroup.svelte'
    import { writable } from 'svelte/store'
    import { default_opts, steps } from './install_selector'
    import { generate_new_disable_items } from './install_selector'
    import type { GroupName, SelectedGroup } from './install_selector'
    import { get_download_configure } from './install_selector'

    /**
     * apply函数
     */
    const event_fun = curry_2(apply_svelte)

    /**
     * 不可使用的项
     */
    const disable_items = writable<Map<string, string[]>>(
        generate_new_disable_items(default_opts)
    )

    /**
     * 当前选中的值
     */
    const selected = writable<SelectedGroup>(default_opts)

    /**
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as GroupName
        selected.update((last) => {
            const new_val = { ...last, [group]: arg.value }
            // 更新disable items
            const disables = generate_new_disable_items(new_val)
            disable_items.set(disables)
            return new_val
        })
    }

    /**
     * 请求下载文件
     */
    function request_download_file() {
        // 取得下载配置
        const cfg = get_download_configure($selected)
        if (cfg !== null) {
            // 找到了合适的情况
            if (cfg.type === 'hyper') {
                // 超链接
                const addr = cfg.addr
                console.log(addr)
            } else if (cfg.type === 'artifact') {
                // github action存档
                const file = cfg.addr
                API.get_workflows_status(file)
                    .then((res) => {
                        console.log(res)
                        return API.get_workflows_runs_artifacts(
                            res.workflow_runs[0].id
                        )
                    })
                    .then((res) => {
                        console.log(res)
                    })
            }
        } else {
            // TODO 没有找到
        }
    }
</script>

<div>
    <Caption caption_text="Download MtFmt" />
    <div class="install-hints">
        {@html InstallText.default}
    </div>
    <ul class="steps">
        {#each steps as step}
            <li>
                <div class="step-name">{step[0]}</div>
                <div class="step-selector">
                    <RadioButtonGroup
                        group_name={step[0]}
                        items={step[1]}
                        default_selected={$selected[step[0]] ?? undefined}
                        diasble_items={$disable_items.get(step[0]) ?? []}
                        on:change={event_fun(on_change_select)}
                    />
                </div>
            </li>
        {/each}
        <li>
            <div class="finally-step-name">Download</div>
            <div class="step-selector">
                <Button on:click={request_download_file}>
                    {JSON.stringify($selected)}
                </Button>
            </div>
        </li>
    </ul>
</div>

<style lang="scss">
    @import '@theme/stylevars';

    .install-hints {
        font-family: $text-font-en;
        letter-spacing: 0.25px;
        line-height: 180%;
        margin-top: 0;
        margin-bottom: 2em;
    }

    .steps {
        padding: 0;
        margin: 0;
        margin-top: 1.5em;
        margin-bottom: 1em;
        list-style-type: none;

        li {
            display: flex;
            flex-wrap: nowrap;
            flex-direction: row;
            align-items: baseline;
            justify-content: flex-start;
            margin-top: 0.5em;
            margin-bottom: 0.5em;
        }
    }

    .finally-step-name,
    .step-name {
        padding: 0.5em 1.5em;
        flex: 0 0 4em;
        text-transform: capitalize;
        overflow: hidden;
    }

    .step-name {
        border-left: 2px solid var(--border-color-middle);
    }

    .step-selector {
        flex-grow: 1;
    }

    .finally-step-name {
        border-left: 2px solid var(--border-color-deep);
    }
</style>
