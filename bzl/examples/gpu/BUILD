load("//bzl/apple:objc.bzl", "nia_objc_library")
load("//bzl/gpu:metal.bzl", "metal_library")

metal_library(
    name = "simple_shaders_mtl",
    srcs = ["simple_shaders.metal"],
    hdrs = ["ShaderTypes.h"],
    tags = [
        "manual",
    ],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:ios": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)

# Only needed for cc_library depending on simple_shaders_mtl.
nia_objc_library(
    name = "hello-metal",
    hdrs = ["ShaderTypes.h"],
    tags = [
        "manual",
    ],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:ios": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [":simple_shaders_mtl"],
)
