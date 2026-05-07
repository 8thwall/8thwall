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
        "//apps/desktop:errors",
        "//apps/desktop:json-response",
        "//apps/desktop:local-project-db",
        "//apps/desktop:project-helpers",
        "//apps/desktop:query-params",
        "//apps/desktop:requests",
        "//apps/desktop:stream-file-response",
        "//apps/desktop/app/file-sync:file-handler-types",
        "//apps/desktop/app/file-sync:paths",
        "//apps/desktop/app/preferences:code-editor",
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
        "//apps/desktop:errors",
        "//apps/desktop:json-response",
        "//apps/desktop:local-project-db",
        "//apps/desktop:local-server",
        "//apps/desktop:query-params",
        "//apps/desktop:requests",
        "//apps/desktop/app/file-sync:create-project-files",
        "//apps/desktop/app/file-sync:paths",
        "//apps/desktop/app/file-sync:project-handler-types",
        "//apps/desktop/app/file-sync:run-commands",
        "//apps/desktop/app/preferences:code-editor",
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
        "//apps/desktop/app:resources",
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
    name = "install-build-package",
    srcs = [
        "install-build-package.ts",
    ],
    deps = [
        "//apps/desktop:errors",
        "//apps/desktop/app/file-sync:run-commands",
    ],
)

js_library(
    name = "run-commands",
    srcs = [
        "run-commands.ts",
    ],
    deps = [
        "//apps/desktop/app:resources",
    ],
)
