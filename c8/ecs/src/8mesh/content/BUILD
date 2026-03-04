load("//bzl/js:js.bzl", "js_library")

package(
    default_visibility = ["//visibility:public"],
)

js_library(
    name = "Frame",
    srcs = [
        "Frame.ts",
    ],
)

js_library(
    name = "MSDFGlyph",
    srcs = [
        "MSDFGlyph.ts",
    ],
)

js_library(
    name = "MSDFText",
    srcs = [
        "MSDFText.ts",
    ],
    deps = [
        "//c8/ecs/src/8mesh/components/core:FontLibrary",
        "//c8/ecs/src/8mesh/content:MSDFGlyph",
        "//c8/ecs/src/8mesh/utils:three-types",
        "//c8/ecs/src/shared:msdf-font-type",
    ],
)
