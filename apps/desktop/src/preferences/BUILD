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
        "//apps/desktop/src/preferences:code-editor",
        "//apps/desktop/src/preferences:local-preferences",
        "//apps/desktop/src/transport:errors",
        "//apps/desktop/src/transport:json-response",
        "//apps/desktop/src/transport:requests",
        "//reality/shared/desktop:preferences-types",
    ],
)

js_library(
    name = "protocol",
    srcs = [
        "protocol.ts",
    ],
    deps = [
        "//apps/desktop/src/preferences:preferences-handler",
    ],
)

js_library(
    name = "code-editor",
    srcs = [
        "code-editor.ts",
    ],
    deps = [
        "//apps/desktop/src/preferences:local-preferences",
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
        "//apps/desktop/src/core:application-state",
    ],
)
