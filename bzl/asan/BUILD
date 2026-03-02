load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")

bool_flag(
    name = "asan",
    build_setting_default = False,
    visibility = [
        "//visibility:public",
    ],
)

config_setting(
    name = "with-asan",
    flag_values = {
        ":asan": "True",
    },
    visibility = [
        "//visibility:public",
    ],
)
