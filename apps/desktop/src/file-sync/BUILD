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
        "//apps/desktop/src/file-sync:file-handler-types",
        "//apps/desktop/src/file-sync:paths",
        "//apps/desktop/src/preferences:code-editor",
        "//apps/desktop/src/project:local-project-db",
        "//apps/desktop/src/project:project-helpers",
        "//apps/desktop/src/transport:errors",
        "//apps/desktop/src/transport:json-response",
        "//apps/desktop/src/transport:query-params",
        "//apps/desktop/src/transport:requests",
        "//apps/desktop/src/transport:stream-file-response",
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
        "//apps/desktop/src/file-sync:create-project-files",
        "//apps/desktop/src/file-sync:paths",
        "//apps/desktop/src/file-sync:project-handler-types",
        "//apps/desktop/src/preferences:code-editor",
        "//apps/desktop/src/project:local-project-db",
        "//apps/desktop/src/project:local-server",
        "//apps/desktop/src/project:run-commands",
        "//apps/desktop/src/transport:errors",
        "//apps/desktop/src/transport:json-response",
        "//apps/desktop/src/transport:query-params",
        "//apps/desktop/src/transport:requests",
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
        "//apps/desktop/src/file-sync:file-handler",
        "//apps/desktop/src/file-sync:paths",
        "//apps/desktop/src/file-sync:project-handler",
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
        "//apps/desktop/src/core:resources",
        "//reality/shared:run-queue",
    ],
)
