load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "audio-mixer",
    srcs = [
        "audio-mixer.js",
    ],
)

js_library(
    name = "canvas-operations",
    srcs = [
        "canvas-operations.ts",
    ],
)

js_library(
    name = "direct-recorder",
    srcs = [
        "direct-recorder.js",
    ],
    deps = [
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src/media:native-recorder",
    ],
)

js_library(
    name = "frame-compositor",
    srcs = [
        "frame-compositor.js",
    ],
    deps = [
        "//reality/app/xr/js/src:canvas",
        "//reality/app/xr/js/src/media:canvas-operations",
    ],
)

js_library(
    name = "native-recorder",
    srcs = [
        "native-recorder.js",
    ],
)

js_library(
    name = "transcoded-recorder",
    srcs = [
        "transcoded-recorder.js",
    ],
    deps = [
        "//reality/app/xr/js/src/media:native-recorder",
        "//reality/app/xr/js/src/media:worker",
    ],
)

js_library(
    name = "wasm-recorder",
    srcs = [
        "wasm-recorder.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/media:worker",
    ],
)

js_library(
    name = "worker",
    srcs = [
        "worker.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:resources",
    ],
)
