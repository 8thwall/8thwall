load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/src/image-targets:image-target-handler",
    ],
)

js_library(
    name = "image-target-handler",
    srcs = [
        "image-target-handler.ts",
    ],
    deps = [
        "//apps/desktop/src/image-targets:image-target-types",
        "//apps/desktop/src/project:local-project-db",
        "//apps/desktop/src/transport:errors",
        "//apps/desktop/src/transport:json-response",
        "//apps/desktop/src/transport:query-params",
        "//apps/desktop/src/transport:requests",
        "//apps/desktop/src/transport:stream-file-response",
        "//apps/image-target-cli/src:apply",
        "//reality/shared:run-queue",
        "//reality/shared/desktop:image-target-api",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "image-target-types",
    srcs = [
        "image-target-types.ts",
    ],
)
