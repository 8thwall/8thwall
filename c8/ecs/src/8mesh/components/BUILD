load("//bzl/js:js.bzl", "js_library")

package(
    default_visibility = ["//visibility:public"],
)

js_library(
    name = "Block",
    srcs = [
        "Block.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:BoxComponent",
        "//c8/ecs/src/8mesh/components/core:InlineManager",
        "//c8/ecs/src/8mesh/components/core:MaterialManager",
        "//c8/ecs/src/8mesh/components/core:MeshUIComponent",
        "//c8/ecs/src/8mesh/content:Frame",
        "//c8/ecs/src/8mesh/utils:mix",
        "//c8/ecs/src/8mesh/utils:set-layers-on-object",
    ],
)

js_library(
    name = "InlineBlock",
    srcs = [
        "InlineBlock.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:BoxComponent",
        "//c8/ecs/src/8mesh/components/core:InlineComponent",
        "//c8/ecs/src/8mesh/components/core:InlineManager",
        "//c8/ecs/src/8mesh/components/core:MaterialManager",
        "//c8/ecs/src/8mesh/components/core:MeshUIComponent",
        "//c8/ecs/src/8mesh/content:Frame",
        "//c8/ecs/src/8mesh/utils:mix",
        "//c8/ecs/src/8mesh/utils:set-layers-on-object",
    ],
)

js_library(
    name = "Keyboard",
    srcs = [
        "Keyboard.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components:Block",
        "//c8/ecs/src/8mesh/components:InlineBlock",
        "//c8/ecs/src/8mesh/components:Text",
        "//c8/ecs/src/8mesh/components/core:BoxComponent",
        "//c8/ecs/src/8mesh/components/core:MeshUIComponent",
        "//c8/ecs/src/8mesh/utils:Keymaps",
        "//c8/ecs/src/8mesh/utils:mix",
    ],
)

js_library(
    name = "Text",
    srcs = [
        "Text.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:FontLibrary",
        "//c8/ecs/src/8mesh/components/core:InlineComponent",
        "//c8/ecs/src/8mesh/components/core:MaterialManager",
        "//c8/ecs/src/8mesh/components/core:MeshUIComponent",
        "//c8/ecs/src/8mesh/components/core:TextManager",
        "//c8/ecs/src/8mesh/utils:deepDelete",
        "//c8/ecs/src/8mesh/utils:mix",
        "//c8/ecs/src/8mesh/utils/inline-layout:Whitespace",
    ],
)
