load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "application-state",
    srcs = [
        "application-state.ts",
    ],
    deps = [
        "//apps/desktop/src/core:resources",
    ],
)

js_library(
    name = "resources",
    srcs = [
        "resources.ts",
    ],
)

js_library(
    name = "desktop-protocol",
    srcs = [
        "desktop-protocol.ts",
    ],
    deps = [
        "//reality/shared/desktop:desktop-protocol-types",
    ],
)
