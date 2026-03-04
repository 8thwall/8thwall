load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//c8/ecs/src/runtime:__subpackages__"])

js_library(
    name = "algorithms",
    srcs = [
        "algorithms.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
)

js_library(
    name = "mat4",
    srcs = [
        "mat4.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math:algorithms",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/runtime/math:vec3",
    ],
)

js_library(
    name = "quat",
    srcs = [
        "quat.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math:algorithms",
        "//c8/ecs/src/runtime/math:vec3",
    ],
)

js_library(
    name = "vec2",
    srcs = [
        "vec2.ts",
    ],
)

js_library(
    name = "vec3",
    srcs = [
        "vec3.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/math:algorithms",
    ],
)

js_library(
    name = "math",
    srcs = [
        "math.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/runtime/math:mat4",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/runtime/math:vec2",
        "//c8/ecs/src/runtime/math:vec3",
    ],
)

js_test(
    name = "math-test",
    size = "small",
    main = [
        "math-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/ecs/src/runtime/math",
    ],
)
