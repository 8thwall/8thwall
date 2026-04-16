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
        "//apps/desktop/app/image-targets:image-target-handler",
    ],
)

js_library(
    name = "image-target-handler",
    srcs = [
        "image-target-handler.ts",
    ],
    deps = [
        "//apps/desktop:errors",
        "//apps/desktop:json-response",
        "//apps/desktop:local-project-db",
        "//apps/desktop:query-params",
        "//apps/desktop:requests",
        "//apps/desktop:stream-file-response",
        "//apps/desktop/app/image-targets:image-target-types",
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
