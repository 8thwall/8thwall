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
        "//apps/desktop/file-sync:file-handler-types",
        "//apps/desktop/file-sync:paths",
        "//apps/desktop/preferences:code-editor",
        "//apps/desktop/project:local-project-db",
        "//apps/desktop/project:project-helpers",
        "//apps/desktop/transport:errors",
        "//apps/desktop/transport:json-response",
        "//apps/desktop/transport:query-params",
        "//apps/desktop/transport:requests",
        "//apps/desktop/transport:stream-file-response",
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
        "//apps/desktop/file-sync:create-project-files",
        "//apps/desktop/file-sync:paths",
        "//apps/desktop/file-sync:project-handler-types",
        "//apps/desktop/preferences:code-editor",
        "//apps/desktop/project:local-project-db",
        "//apps/desktop/project:local-server",
        "//apps/desktop/project:run-commands",
        "//apps/desktop/transport:errors",
        "//apps/desktop/transport:json-response",
        "//apps/desktop/transport:query-params",
        "//apps/desktop/transport:requests",
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
        "//apps/desktop/file-sync:file-handler",
        "//apps/desktop/file-sync:paths",
        "//apps/desktop/file-sync:project-handler",
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
        "//apps/desktop/core:resources",
        "//reality/shared:run-queue",
    ],
)
