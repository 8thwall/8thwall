load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "local-proxy",
    srcs = [
        "local-proxy.ts",
    ],
)

js_library(
    name = "local-server",
    srcs = [
        "local-server.ts",
    ],
    deps = [
        "//apps/desktop/dev8-socket:dev8-socket-server",
        "//apps/desktop/project:local-proxy",
        "//apps/desktop/project:run-commands",
        "//apps/desktop/system-log:listeners",
        "//c8/cli:ip",
        "//reality/cloud/xrhome/src/shared:time-utils",
    ],
)

js_library(
    name = "process",
    srcs = [
        "process.ts",
    ],
)

js_library(
    name = "run-commands",
    srcs = [
        "run-commands.ts",
    ],
    deps = [
        "//apps/desktop/core:resources",
        "//apps/desktop/project:process",
        "//apps/desktop/system-log:listeners",
        "//reality/cloud/xrhome/src/shared:time-utils",
    ],
)

js_library(
    name = "local-project-db",
    srcs = [
        "local-project-db.ts",
    ],
    deps = [
        "//apps/desktop/core:application-state",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "project-helpers",
    srcs = [
        "project-helpers.ts",
    ],
    deps = [
        "//reality/shared/desktop:unix-path",
    ],
)
