load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "create-controllers",
    srcs = [
        "create-controllers.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-controller-model-factory",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-hand-model-factory",
    ],
)

js_library(
    name = "desktop-camera-manager",
    srcs = [
        "desktop-camera-manager.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:transforms",
    ],
)

js_library(
    name = "motion-controllers",
    srcs = [
        "motion-controllers.js",
    ],
)

js_library(
    name = "scene-graph",
    srcs = [
        "scene-graph.ts",
    ],
)

js_library(
    name = "transforms",
    srcs = [
        "transforms.ts",
    ],
)

js_library(
    name = "void-space",
    srcs = [
        "void-space.ts",
    ],
)

js_library(
    name = "xr-camera-attachment-manager",
    srcs = [
        "xr-camera-attachment-manager.ts",
    ],
)

js_library(
    name = "xr-controller-model-factory",
    srcs = [
        "xr-controller-model-factory.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:motion-controllers",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-controller-model",
    ],
)

js_library(
    name = "xr-controller-model",
    srcs = [
        "xr-controller-model.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:motion-controllers",
    ],
)

js_library(
    name = "xr-hand-mesh-model",
    srcs = [
        "xr-hand-mesh-model.js",
    ],
)

js_library(
    name = "xr-hand-model-factory",
    srcs = [
        "xr-hand-model-factory.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-hand-mesh-model",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-hand-model",
    ],
)

js_library(
    name = "xr-hand-model",
    srcs = [
        "xr-hand-model.ts",
    ],
)

js_library(
    name = "xr-input-manager",
    srcs = [
        "xr-input-manager.js",
    ],
    deps = [
        "//reality/app/xr/js/src/webxrcontrollersthreejs:create-controllers",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:scene-graph",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:transforms",
        "//reality/app/xr/js/src/webxrcontrollersthreejs:xr-camera-attachment-manager",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:dom-tablet",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:dom-tablet-alignment",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:dom-wrist-model",
        "//reality/app/xr/js/src/webxrcontrollersthreejs/tablet:interaction-provider",
    ],
)
