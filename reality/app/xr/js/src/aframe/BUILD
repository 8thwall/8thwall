load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "layer-scene-primitive",
    srcs = [
        "layer-scene-primitive.ts",
    ],
)

js_library(
    name = "common",
    srcs = [
        "common.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:void-space",
    ],
)

js_library(
    name = "xr-config-component",
    srcs = [
        "xr-config-component.ts",
    ],
    deps = [
        "//reality/app/xr/js/src:geometry-conversions",
        "//reality/app/xr/js/src:js-device",
        "//reality/app/xr/js/src:permissions-helper",
        "//reality/app/xr/js/src:prevent-default-touch",
        "//reality/app/xr/js/src:webxr-compatibility",
        "//reality/app/xr/js/src/aframe:common",
        "//reality/app/xr/js/src/types:callbacks",
        "//reality/app/xr/js/src/types:pipeline",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:desktop-camera-manager",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:void-space",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-input-manager",
    ],
)

js_library(
    name = "xr-web-component",
    srcs = [
        "xr-web-component.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/aframe:common",
    ],
)

js_library(
    name = "xr-layer-scene-component",
    srcs = [
        "xr-layer-scene-component.ts",
    ],
    deps = [
        "//reality/app/xr/js/src/types:layers",
    ],
)
