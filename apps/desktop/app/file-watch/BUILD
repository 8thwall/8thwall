load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "api",
    srcs = [
        "api.ts",
    ],
    deps = [
        "//reality/shared/desktop:electron-api",
    ],
)

js_library(
    name = "ports",
    srcs = [
        "ports.ts",
    ],
    deps = [
        "//apps/desktop/app/file-watch:file-watcher",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "file-watcher",
    srcs = [
        "file-watcher.ts",
    ],
    deps = [
        "//apps/desktop:local-project-db",
        "//apps/desktop:project-helpers",
        "//reality/shared/desktop:local-sync-types",
        "//reality/shared/desktop:unix-path",
    ],
)
