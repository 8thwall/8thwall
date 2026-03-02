cc_library(
    name = "nodejs-nan",
    hdrs = glob(
        ["*.h"],
        exclude = ["*_inl.h"],
    ),
    includes = ["."],
    textual_hdrs = glob([
        "*_inl.h",
    ]),
    visibility = ["//visibility:public"],
    deps = [
        "@the8thwall//bzl/node:node-addons",
    ],
)
