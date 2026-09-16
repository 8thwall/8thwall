load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "application-state",
    srcs = [
        "application-state.ts",
    ],
)

js_library(
    name = "local-project-db",
    srcs = [
        "local-project-db.ts",
    ],
    deps = [
        "//apps/desktop/app/storage:application-state",
        "//reality/shared/desktop:local-sync-types",
    ],
)
