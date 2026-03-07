load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//c8/ecs/src/runtime:__subpackages__"])

js_library(
    name = "hidden-component",
    srcs = [
        "hidden-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
    ],
)

js_library(
    name = "ui-component",
    srcs = [
        "ui-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:ui-constants",
    ],
)

js_library(
    name = "face-component",
    srcs = [
        "face-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_library(
    name = "camera-component",
    srcs = [
        "camera-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:camera-constants",
    ],
)

js_library(
    name = "light-component",
    srcs = [
        "light-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:light-constants",
    ],
)

js_library(
    name = "persistent-component",
    srcs = [
        "persistent-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
    ],
)

js_library(
    name = "splat-component",
    srcs = [
        "splat-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_library(
    name = "shadow-component",
    srcs = [
        "shadow-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_library(
    name = "audio-component",
    srcs = [
        "audio-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:audio-constants",
    ],
)

js_library(
    name = "geometry-components",
    srcs = [
        "geometry-components.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_library(
    name = "gltf-component",
    srcs = [
        "gltf-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_library(
    name = "transform-components",
    srcs = [
        "transform-components.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:transform-constants",
    ],
)

js_library(
    name = "material-components",
    srcs = [
        "material-components.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:material-constants",
    ],
)

js_library(
    name = "image-target-component",
    srcs = [
        "image-target-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
    ],
)

js_test(
    name = "transform-components-test",
    size = "small",
    esnext = 1,
    externalize_npm = 1,
    main = [
        "transform-components-test.ts",
    ],
    npm_rule = "@npm-ecs//:npm-ecs",
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime:test-env",
        "//c8/ecs/src/runtime:test-runtime-lib",
        "//c8/ecs/src/runtime:world",
    ],
)

js_library(
    name = "video-component",
    srcs = [
        "video-component.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:types",
        "//c8/ecs/src/shared:video-constants",
    ],
)
