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
        "//apps/desktop/preferences:code-editor",
        "//apps/desktop/preferences:local-preferences",
        "//apps/desktop/transport:errors",
        "//apps/desktop/transport:json-response",
        "//apps/desktop/transport:requests",
        "//reality/shared/desktop:preferences-types",
    ],
)

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/preferences:preferences-handler",
    ],
)

js_library(
    name = "code-editor",
    srcs = [
        "code-editor.ts",
    ],
    deps = [
        "//apps/desktop/preferences:local-preferences",
        "//reality/shared:run-queue",
        "//reality/shared/desktop:preferences-types",
    ],
)

js_library(
    name = "local-preferences",
    srcs = [
        "local-preferences.ts",
    ],
    deps = [
        "//apps/desktop/core:application-state",
    ],
)
