load("@bazel_skylib//lib:selects.bzl", "selects")

filegroup(
    name = "shadercc",
    srcs = select({
        "//bzl/conditions:windows": ["shadercc.cmd"],
        "//conditions:default": ["shadercc.sh"],
    }),
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "osx-without-angle",
    match_all = [
        "//bzl/conditions:osx",
        "//c8/pixels/opengl:without-angle",
    ],
)
