<!-- 
#  SPDX-License-Identifier: LGPL-3.0
-->
<script lang="ts">
    import * as API from '$lib/github_api'
    import Caption from './Caption.svelte'
    import { curry_2 } from '$lib/fp/fun'
    import { writable } from 'svelte/store'
    import * as Inner from './install_selector'
    import { apply_svelte } from '$lib/fp/apply'
    import { record_to_map } from '$lib/fp/cast'
    import GithubAPI from '@text/github_api.json'
    import DownloadData from '@text/download.json'
    import * as InstallText from '@text/install.txt?raw'
    import RadioButtonGroup from '@comp/RadioButtonGroup.svelte'

    /**
     * 下载文件的信息
     */
    type DownloadFileInfo = {
        addr: string
        name: string
    }

    /**
     * apply函数
     */
    const event_fun = curry_2(apply_svelte)

    /**
     * 不可使用的项
     */
    const disable_items = writable<Map<string, string[]>>(
        Inner.generate_new_disable_items(Inner.default_opts)
    )

    /**
     * 当前选中的值
     */
    const selected = writable<Inner.SelectedGroup>(Inner.default_opts)

    /**
     * 当前的下载配置
     */
    const download_opts = writable<Inner.DownloadInfo | null>(
        Inner.get_download_configure(Inner.default_opts)
    )

    /**
     * workflow的base url
     */
    const workflow_base = GithubAPI.workflow_base_url

    /**
     * build opts的选项
     */
    const opts_map = record_to_map<string, string>(
        DownloadData.configure,
        (s) => s
    )

    /**
     * 选中项改变
     */
    function on_change_select(arg: { value: string; group: string }) {
        const group = arg.group as Inner.GroupName
        selected.update((last) => {
            const new_val = { ...last, [group]: arg.value }
            // 更新disable items
            const disables = Inner.generate_new_disable_items(new_val)
            disable_items.set(disables)
            // 更新download opts
            const opts = Inner.get_download_configure(new_val)
            download_opts.set(opts)
            // 尝试去gayhub上把下载信息整回来
            // TODO
            return new_val
        })
    }

    /**
     * 请求github上的文件
     */
    function request_action_file(
        file: string,
        name?: string
    ): Promise<DownloadFileInfo> {
        return API.get_workflows_status(file)
            .then((res) => {
                return new Promise<API.WorkflowStatus>((resolve, reject) => {
                    if (res.total_count === 0) {
                        reject('No file found')
                    } else {
                        resolve(res)
                    }
                })
            })
            .then((res) => {
                return API.get_workflows_runs_artifacts(res.workflow_runs[0].id)
            })
            .then((res) => {
                return new Promise<API.WorkflowRunsArtifacts>((rsv, rej) => {
                    const artifacts = res.artifacts
                        .filter((s) => s.name === (name ?? s.name))
                        .filter((s) => !s.expired)
                    if (artifacts.length === 0) {
                        rej('No suitable artifact be found')
                    } else if (artifacts.length > 1) {
                        rej('More than one suitable artifacts be found')
                    } else {
                        rsv(res)
                    }
                })
            })
            .then((res) => {
                const artifact = res.artifacts[0]
                return {
                    addr: artifact.archive_download_url,
                    name: artifact.name,
                }
            })
    }

    /**
     * 请求超链接的文件
     */
    function request_url_file(url: string): Promise<DownloadFileInfo> {
        return new Promise((resolve) => {
            resolve({
                addr: url,
                name: url,
            })
        })
    }

    /**
     * 弹出下载文件的那个东东
     */
    function pop_download(addr: string) {
        // TODO
    }

    /**
     * 请求下载文件
     */
    function request_download_file() {
        const info = $download_opts
        // 取得下载配置
        if (info === null || info.type !== 'artifact') {
            throw 'unreachable'
        }
        const cfg = info as Inner.DownloadInfo
        let file: Promise<DownloadFileInfo>
        if (cfg.type === 'hyper') {
            // 超链接
            file = request_url_file(cfg.addr)
        } else if (cfg.type === 'artifact') {
            // github action存档
            file = request_action_file(cfg.addr, cfg.file)
        } else {
            throw 'unreachable'
        }
        file.then((res) => {
            console.log(res)
        }).catch((err) => {
            // TODO ERR
            console.log(err)
        })
    }

    /**
     * 直接下载
     */
    function direct_download() {
        const info = $download_opts
        // 取得下载配置
        if (info === null || info.type !== 'hyper') {
            throw 'unreachable'
        }
        const cfg = info as Inner.DownloadInfo
        pop_download(cfg.addr)
    }
</script>

<div>
    <Caption caption_text="Download MtFmt" />
    <div class="install-hints">
        {@html InstallText.default}
    </div>
    <ul class="steps">
        {#each Inner.steps as step}
            <li class="steps-li">
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
        <li class="steps-li">
            <div class="step-name">Configure</div>
            <div class="step-selector">
                {#if $download_opts === null}
                    <div class="error-hints">
                        <span>No available downloads</span>
                    </div>
                {:else}
                    <ul class="cfg-hints">
                        {#if $download_opts.type === 'hyper'}
                            <li>Source: Hyper-link</li>
                            <li>
                                <span>Address:</span>
                                <a href={$download_opts.addr}>
                                    {$download_opts.addr}
                                </a>
                            </li>
                        {:else}
                            <li>Source: Github action artifacts</li>
                            {#each Object.entries($download_opts.build ?? {}) as build_opt}
                                <li>
                                    <div class="build-opts">
                                        <div class="opt-content">#define</div>
                                        <div class="opt-content-name">
                                            {opts_map.get(build_opt[0]) ?? ''}
                                        </div>
                                        <div class="opt-content">
                                            {Number(build_opt[1])}
                                        </div>
                                    </div>
                                </li>
                            {/each}
                        {/if}
                    </ul>
                {/if}
            </div>
        </li>
        <li class="steps-li">
            <div class="finally-step-name">Download</div>
            <div class="step-selector">
                {#if $download_opts === null}
                    <div class="error-hints">
                        <div>No available downloads</div>
                    </div>
                {:else if $download_opts.type === 'hyper'}
                    <a
                        class="downlad-button-a"
                        href={$download_opts.addr}
                        download
                    >
                        Download
                    </a>
                {:else}
                    <a class="downlad-button-a" href="/" download> Download </a>
                {/if}
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
    }

    .steps-li {
        display: flex;
        flex-wrap: nowrap;
        flex-direction: row;
        align-items: baseline;
        justify-content: flex-start;
        margin-top: 0.5em;
        margin-bottom: 0.5em;
        overflow: hidden;
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

    .error-hints {
        font-style: oblique;
    }

    .cfg-hints {
        list-style-type: circle;
        padding: 0;
        margin: 0;
        margin-left: 1.2em;
    }

    .build-opts {
        display: flex;
        flex-wrap: wrap;
        flex-direction: row;
        gap: 0.5em;
    }

    .opt-content {
        flex: 0 0 auto;
    }

    .opt-content-name {
        flex: 0 0 16em;
    }

    .finally-step-name {
        border-left: 2px solid var(--border-color-deep);
    }

    .downlad-button-a {
        padding: 0;
        margin: 0;
        padding: 0.6em 0.8em;
        display: block;
        text-decoration: none;
        max-width: 6em;
        text-align: center;

        // 边框
        outline: 0;
        border: none;
        border-radius: 2px;

        // 颜色
        color: inherit;
        font-family: $text-font-en;
        font-size: 1.05rem;
        line-height: 1.05rem;
        background-color: var(--button2-bg-color);

        &:hover {
            background-color: var(--button2-hover-bg-color);
        }
    }
</style>
