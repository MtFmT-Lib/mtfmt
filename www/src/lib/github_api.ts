// SPDX-License-Identifier: LGPL-3.0
import axios from 'axios'
import GithubAPI from '@text/github_api.json'

/**
 * workflow status的response type (部分)
 * 
 * 考虑用 https://github.com/bcherny/json-schema-to-typescript
 * 
 * 但是似乎要弄 vite 插件, 暂时不折腾吧
 * 
 * 参考:
 * https://docs.github.com/en/rest/actions/workflow-runs
 *      ?apiVersion=2022-11-28#list-workflow-runs-for-a-workflow
 */
export type WorkflowStatus = {
    total_count: number,
    workflow_runs: {
        id: number,
        name: string,
        status: string,
        conclusion: string,
        url: string,
        head_commit: {
            id: string,
            timestamp: string
        }
    }[]
}

/**
 * 存档信息
 * 
 * 参考:
 * https://docs.github.com/en/rest/actions/artifacts
 *      ?apiVersion=2022-11-28#list-workflow-run-artifacts
 */
export type WorkflowRunsArtifacts = {
    id: number,
    name: string,
    archive_download_url: string,
    created_at: string,
    expires_at: string
}

/**
 * 取得workflows的status
 */
export function get_workflows_status(file: string): Promise<WorkflowStatus> {
    const url = get_workflows_status_api_url(file)
    return axios.get<WorkflowStatus>(url).then(res => res.data)
}

/**
 * 取得workflows的artifacts
 */
export function get_workflows_runs_artifacts(id: number): Promise<WorkflowRunsArtifacts> {
    const url = get_runs_artifacts_api_url(id)
    return axios.get<WorkflowRunsArtifacts>(url).then(res => res.data)
}

/**
 * 取得workflows run的artifacts的url
 */
function get_runs_artifacts_api_url(id: number): string {
    return GithubAPI['artifact-template'].replace('$runs_id$', id.toString())
}

/**
 * 取得workflows file的api的url
 */
function get_workflows_status_api_url(file: string): string {
    return GithubAPI['action-template'].replace('$workflows$', file)
}
