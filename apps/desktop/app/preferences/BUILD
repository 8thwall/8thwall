load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "preferences-handler",
    srcs = [
        "preferences-handler.ts",
    ],
    deps = [
        "//apps/desktop:errors",
        "//apps/desktop:json-response",
        "//apps/desktop:local-preferences",
        "//apps/desktop:requests",
        "//apps/desktop/app/preferences:code-editor",
        "//reality/shared/desktop:preferences-types",
    ],
)

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/app/preferences:preferences-handler",
    ],
)

js_library(
    name = "code-editor",
    srcs = [
        "code-editor.ts",
    ],
    deps = [
        "//apps/desktop:local-preferences",
        "//reality/shared:run-queue",
        "//reality/shared/desktop:preferences-types",
    ],
)
