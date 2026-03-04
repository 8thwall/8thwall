load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//c8/ecs/src/runtime:__subpackages__"])

js_library(
    name = "audio-system",
    srcs = [
        "audio-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:audio-manager",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "camera-system",
    srcs = [
        "camera-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera",
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "geometry-system",
    srcs = [
        "geometry-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "gltf-system",
    srcs = [
        "gltf-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:get-gltf-vertices",
        "//c8/ecs/src/runtime:loaders",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:physics",
        "//c8/ecs/src/runtime:texture-constants",
        "//c8/ecs/src/runtime:texture-types",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:video-texture",
        "//c8/ecs/src/runtime:video-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:gltf-constants",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "hidden-system",
    srcs = [
        "hidden-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "light-systems",
    srcs = [
        "light-systems.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:lights",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:light-constants",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "splat-system",
    srcs = [
        "splat-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:load-script",
        "//c8/ecs/src/shared:resources",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "system-helper",
    srcs = [
        "system-helper.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:query",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "transform-system",
    srcs = [
        "transform-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:query",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "ui-system",
    srcs = [
        "ui-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:3d-ui",
        "//c8/ecs/src/runtime:8mesh",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:disabled",
        "//c8/ecs/src/runtime:events-types",
        "//c8/ecs/src/runtime:input",
        "//c8/ecs/src/runtime:matrix-refresh",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:ui",
        "//c8/ecs/src/runtime:ui-events",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime:world-attribute",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:custom-sorting",
        "//c8/ecs/src/shared:determine-ui-order",
        "//c8/ecs/src/shared:find-component",
        "//c8/ecs/src/shared:find-root-component",
        "//c8/ecs/src/shared:flex-enum",
        "//c8/ecs/src/shared:flex-styles",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:three-layers",
        "//c8/ecs/src/shared:ui-constants",
        "//c8/ecs/src/shared:validate-number",
    ],
)

js_library(
    name = "image-target-system",
    srcs = [
        "image-target-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "face-system",
    srcs = [
        "face-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:camera-events",
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/math",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:face-mesh-data",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "video-system",
    srcs = [
        "video-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "video-material-system",
    srcs = [
        "video-material-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:material-systems-helpers",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:material-constants",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "hider-material-system",
    srcs = [
        "hider-material-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:material-systems-helpers",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "material-system",
    srcs = [
        "material-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-side",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:material-systems-helpers",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "material-systems-helpers",
    srcs = [
        "material-systems-helpers.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:assets",
        "//c8/ecs/src/runtime:dispose",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:three-wrapping",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:video-texture",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
        "//c8/ecs/src/shared:video-constants",
    ],
)

js_library(
    name = "shadow-material-system",
    srcs = [
        "shadow-material-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:event-ids",
        "//c8/ecs/src/runtime:events",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-side",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:material-systems-helpers",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:schema",
    ],
)

js_library(
    name = "unlit-material-system",
    srcs = [
        "unlit-material-system.ts",
    ],
    deps = [
        "//c8/ecs/src/runtime:components",
        "//c8/ecs/src/runtime:three",
        "//c8/ecs/src/runtime:three-side",
        "//c8/ecs/src/runtime:three-types",
        "//c8/ecs/src/runtime:video-manager",
        "//c8/ecs/src/runtime:world",
        "//c8/ecs/src/runtime/systems:material-systems-helpers",
        "//c8/ecs/src/runtime/systems:system-helper",
        "//c8/ecs/src/shared:scene-graph",
        "//c8/ecs/src/shared:schema",
    ],
)
