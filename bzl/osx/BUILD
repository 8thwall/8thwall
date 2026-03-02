load("@bazel_skylib//lib:selects.bzl", "selects")

config_setting(
    name = "darwin-unspecified",
    values = {"cpu": "darwin"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "darwin-specified",
    values = {"cpu": "darwin_x86_64"},
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "x86_64",
    match_any = [
        ":darwin-unspecified",
        ":darwin-specified",
    ],
)
