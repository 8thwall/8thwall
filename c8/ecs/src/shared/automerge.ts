// NOTE(christoph): Doing something a little sneaky here because something is stopping bazel from
// resolving the "@automerge/automerge/next" import, which is supposed to be resolved to
// "@automerge/automerge/dist/cjs/next.js" for example.

// This is the code used by vscode/standard webpack, but in bazel we're generating a different
// version of this file in "automerge-gen" which is used in bazel.

// @inliner-off
export * from '@automerge/automerge/next'
