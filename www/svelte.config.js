import path from 'path'
import adapter from '@sveltejs/adapter-static'
import { vitePreprocess } from '@sveltejs/kit/vite'

/** @type {import('@sveltejs/kit').Config} */
const config = {
    // Consult https://kit.svelte.dev/docs/integrations#preprocessors
    // for more information about preprocessors
    preprocess: vitePreprocess(),

    kit: {
        // adapter-auto only supports some environments, see https://kit.svelte.dev/docs/adapter-auto for a list.
        // If your environment is not supported or you settled on a specific environment, switch out the adapter.
        // See https://kit.svelte.dev/docs/adapters for more information about adapters.
        adapter: adapter({
            pages: 'dist',
            assets: 'dist',
            strict: true,
            precompress: false
        }),
        alias: {
            '@comp': path.resolve('./src/lib/c'),
            '@style': path.resolve('./src/themes/webpages'),
            '@theme': path.resolve('./src/themes/common'),
        }
    }
}

export default config
