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
        "//apps/desktop/app/plumbing:errors",
        "//apps/desktop/app/plumbing:json-response",
        "//apps/desktop/app/plumbing:requests",
        "//apps/desktop/app/preferences:code-editor",
        "//apps/desktop/app/preferences:local-preferences",
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
        "//apps/desktop/app/preferences:local-preferences",
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
        "//apps/desktop/app/storage:application-state",
    ],
)
