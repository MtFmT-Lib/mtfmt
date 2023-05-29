// See https://kit.svelte.dev/docs/types#app
// for information about these interfaces
declare global {
    namespace App {
        // interface Error {}
        // interface Locals {}
        // interface PageData {}
        // interface Platform {}

        // Markdown
        declare module '*.md' {
            const toc: { level: string, content: string }[]

            const html: string

            export { toc, html }
        }
    }
}

export { }
