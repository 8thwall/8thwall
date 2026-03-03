load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "constants",
    srcs = [
        "constants.ts",
    ],
)

js_library(
    name = "coordinates",
    srcs = [
        "coordinates.ts",
    ],
)

js_library(
    name = "dom-tablet-alignment",
    srcs = [
        "dom-tablet-alignment.ts",
    ],
)

js_library(
    name = "dom-tablet-model",
    srcs = [
        "dom-tablet-model.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src:threejs-support",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:scene-graph",
    ],
)

js_library(
    name = "dom-tablet",
    srcs = [
        "dom-tablet.js",
    ],
    deps = [
        "//reality/app/xr/js/src/third_party/html-to-image",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:scene-graph",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:constants",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:coordinates",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:dom-tablet-model",
    ],
)

js_library(
    name = "dom-wrist-model",
    srcs = [
        "dom-wrist-model.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:resources",
        "//reality/app/xr/js/src:threejs-support",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:constants",
    ],
)

js_library(
    name = "interaction-provider",
    srcs = [
        "interaction-provider.js",
    ],
)
