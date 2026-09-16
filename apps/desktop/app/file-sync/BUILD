load("//bzl/js:js-stringloader.bzl", "js_stringloader")
load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "file-handler",
    srcs = [
        "file-handler.ts",
    ],
    deps = [
        "//apps/desktop:constants",
        "//apps/desktop/app/file-sync:file-handler-types",
        "//apps/desktop/app/file-sync:paths",
        "//apps/desktop/app/file-sync:project-helpers",
        "//apps/desktop/app/plumbing:errors",
        "//apps/desktop/app/plumbing:json-response",
        "//apps/desktop/app/plumbing:query-params",
        "//apps/desktop/app/plumbing:requests",
        "//apps/desktop/app/plumbing:stream-file-response",
        "//apps/desktop/app/preferences:code-editor",
        "//apps/desktop/app/storage:local-project-db",
        "//reality/shared/desktop:local-sync-types",
        "//reality/shared/desktop:unix-path",
    ],
)

js_library(
    name = "project-handler",
    srcs = [
        "project-handler.ts",
    ],
    deps = [
        "//apps/desktop/app/file-sync:create-project-files",
        "//apps/desktop/app/file-sync:local-server",
        "//apps/desktop/app/file-sync:paths",
        "//apps/desktop/app/file-sync:project-handler-types",
        "//apps/desktop/app/file-sync:run-commands",
        "//apps/desktop/app/plumbing:errors",
        "//apps/desktop/app/plumbing:json-response",
        "//apps/desktop/app/plumbing:query-params",
        "//apps/desktop/app/plumbing:requests",
        "//apps/desktop/app/preferences:code-editor",
        "//apps/desktop/app/storage:local-project-db",
        "//c8/ecs/src/shared:runtime-version",
        "//reality/shared:run-queue",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/app/file-sync:file-handler",
        "//apps/desktop/app/file-sync:paths",
        "//apps/desktop/app/file-sync:project-handler",
    ],
)

js_library(
    name = "paths",
    srcs = [
        "paths.ts",
    ],
)

js_library(
    name = "file-handler-types",
    srcs = [
        "file-handler-types.ts",
    ],
)

js_library(
    name = "project-handler-types",
    srcs = [
        "project-handler-types.ts",
    ],
)

js_library(
    name = "create-project-files",
    srcs = [
        "create-project-files.ts",
    ],
    deps = [
        "//apps/desktop/app/native:resources",
        "//reality/shared:run-queue",
    ],
)

js_stringloader(
    name = "studio-tsconfig",
    srcs = [
        "//apps/desktop/app/file-sync:studio-tsconfig.json",
    ],
)

js_library(
    name = "run-commands",
    srcs = [
        "run-commands.ts",
    ],
    deps = [
        "//apps/desktop/app/file-sync:process",
        "//apps/desktop/app/native:resources",
        "//apps/desktop/app/system-log:listeners",
        "//reality/cloud/xrhome/src/shared:time-utils",
    ],
)

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
        "//apps/desktop:constants",
        "//apps/desktop/app/dev8-socket:dev8-socket-server",
        "//apps/desktop/app/file-sync:local-proxy",
        "//apps/desktop/app/file-sync:run-commands",
        "//apps/desktop/app/system-log:listeners",
        "//c8/cli:ip",
    ],
)

js_library(
    name = "process",
    srcs = [
        "process.ts",
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
