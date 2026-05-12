load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//apps/desktop:__subpackages__",
])

js_library(
    name = "api",
    srcs = [
        "api.ts",
    ],
    deps = [
        "//reality/shared/desktop:electron-api",
        "//reality/shared/desktop:system-log-types",
    ],
)

js_library(
    name = "listeners",
    srcs = [
        "listeners.ts",
    ],
    deps = [
        "//reality/shared/desktop:system-log-types",
    ],
)

js_library(
    name = "ports",
    srcs = [
        "ports.ts",
    ],
    deps = [
        "//apps/desktop/app/system-log:listeners",
        "//reality/shared/desktop:system-log-types",
    ],
)
