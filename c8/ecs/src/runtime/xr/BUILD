load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "xr-ecs-types",
    srcs = [
        "xr-ecs-types.ts",
    ],
)

js_library(
    name = "xr-camera-pipeline",
    srcs = [
        "xr-camera-pipeline.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/xr:xr-camera-pipeline-types",
        "//c8/ecs/src/runtime/xr:xr-ecs-types",
        "//c8/ecs/src/runtime/xr:xr-helpers",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "xr-face-components",
    srcs = [
        "xr-face-components.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:registry",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/shared:face-mesh-data",
        "//c8/ecs/src/shared:instanced",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "xr-manager",
    srcs = [
        "xr-manager.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math:quat",
        "//c8/ecs/src/runtime/xr:xr-camera-pipeline",
        "//c8/ecs/src/runtime/xr:xr-ecs-types",
        "//c8/ecs/src/runtime/xr:xr-manager-types",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:xr-config",
    ],
)

js_library(
    name = "xr-manager-types",
    srcs = [
        "xr-manager-types.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime/xr:xr-camera-pipeline-types",
        "//c8/ecs/src/runtime/xr:xr-ecs-types",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "xr-camera-pipeline-types",
    srcs = [
        "xr-camera-pipeline-types.ts",
    ],
)

js_library(
    name = "xr-helpers",
    srcs = [
        "xr-helpers.ts",
    ],
)
