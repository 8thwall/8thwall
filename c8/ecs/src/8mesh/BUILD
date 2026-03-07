load("//bzl/js:js.bzl", "js_library")

js_library(
    name = "three-mesh-ui",
    srcs = [
        "three-mesh-ui.d.ts",
        "three-mesh-ui.js",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components:Block",
        "//c8/ecs/src/8mesh/components:InlineBlock",
        "//c8/ecs/src/8mesh/components:Keyboard",
        "//c8/ecs/src/8mesh/components:Text",
        "//c8/ecs/src/8mesh/components/core:FontLibrary",
        "//c8/ecs/src/8mesh/components/core:UpdateManager",
        "//c8/ecs/src/8mesh/utils:three-types",
        "//c8/ecs/src/8mesh/utils/block-layout:AlignItems",
        "//c8/ecs/src/8mesh/utils/block-layout:ContentDirection",
        "//c8/ecs/src/8mesh/utils/block-layout:JustifyContent",
        "//c8/ecs/src/8mesh/utils/inline-layout:TextAlign",
        "//c8/ecs/src/8mesh/utils/inline-layout:Whitespace",
    ],
)
