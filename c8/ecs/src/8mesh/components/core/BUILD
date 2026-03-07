load("//bzl/js:js.bzl", "js_library")

package(
    default_visibility = ["//visibility:public"],
)

js_library(
    name = "BoxComponent",
    srcs = [
        "BoxComponent.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/utils/block-layout:AlignItems",
        "//c8/ecs/src/8mesh/utils/block-layout:ContentDirection",
        "//c8/ecs/src/8mesh/utils/block-layout:JustifyContent",
    ],
)

js_library(
    name = "FontLibrary",
    srcs = [
        "FontLibrary.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/utils:Defaults",
        "//c8/ecs/src/8mesh/utils:three-types",
        "//c8/ecs/src/shared:msdf-font-type",
    ],
)

js_library(
    name = "InlineComponent",
    srcs = [
        "InlineComponent.ts",
    ],
)

js_library(
    name = "InlineManager",
    srcs = [
        "InlineManager.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/utils/inline-layout:TextAlign",
        "//c8/ecs/src/8mesh/utils/inline-layout:Whitespace",
    ],
)

js_library(
    name = "MaterialManager",
    srcs = [
        "MaterialManager.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:FontLibrary",
        "//c8/ecs/src/8mesh/components/shaders:background-fragment",
        "//c8/ecs/src/8mesh/components/shaders:background-vertex",
        "//c8/ecs/src/8mesh/components/shaders:text-fragment",
        "//c8/ecs/src/8mesh/components/shaders:text-vertex",
        "//c8/ecs/src/8mesh/utils:Defaults",
        "//c8/ecs/src/8mesh/utils:background-size",
        "//c8/ecs/src/8mesh/utils:three-types",
    ],
)

js_library(
    name = "MeshUIComponent",
    srcs = [
        "MeshUIComponent.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:FontLibrary",
        "//c8/ecs/src/8mesh/components/core:UpdateManager",
        "//c8/ecs/src/8mesh/utils:Defaults",
        "//c8/ecs/src/8mesh/utils:set-layers-on-object",
        "//c8/ecs/src/8mesh/utils/block-layout:AlignItems",
        "//c8/ecs/src/8mesh/utils/inline-layout:Whitespace",
    ],
)

js_library(
    name = "TextManager",
    srcs = [
        "TextManager.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/content:MSDFText",
        "//c8/ecs/src/8mesh/utils:set-layers-on-object",
    ],
)

js_library(
    name = "UpdateManager",
    srcs = [
        "UpdateManager.ts",
    ],
)
