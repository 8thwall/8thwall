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
        "//apps/desktop/src/dev8-socket:dev8-socket-server",
        "//apps/desktop/src/project:local-proxy",
        "//apps/desktop/src/project:run-commands",
        "//apps/desktop/src/system-log:listeners",
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
        "//apps/desktop/src/core:resources",
        "//apps/desktop/src/project:process",
        "//apps/desktop/src/system-log:listeners",
        "//reality/cloud/xrhome/src/shared:time-utils",
    ],
)

js_library(
    name = "local-project-db",
    srcs = [
        "local-project-db.ts",
    ],
    deps = [
        "//apps/desktop/src/core:application-state",
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
