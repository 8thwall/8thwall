load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//reality/cloud:shared"])

js_library(
    name = "local-sync-types",
    srcs = [
        "local-sync-types.ts",
    ],
    deps = [
        "//reality/shared/desktop:unix-path",
    ],
)

js_library(
    name = "electron-api",
    srcs = [
        "electron-api.ts",
    ],
    deps = [
        "//reality/shared/desktop:desktop-protocol-types",
        "//reality/shared/desktop:local-sync-types",
    ],
)

js_library(
    name = "desktop-protocol-types",
    srcs = [
        "desktop-protocol-types.ts",
    ],
)

js_library(
    name = "create-electron-url",
    srcs = [
        "create-electron-url.ts",
    ],
)

js_library(
    name = "preferences-types",
    srcs = [
        "preferences-types.ts",
    ],
)

js_library(
    name = "unix-path",
    srcs = [
        "unix-path.ts",
    ],
)

js_library(
    name = "image-target-api",
    srcs = [
        "image-target-api.ts",
    ],
)
