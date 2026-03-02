load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

# When building a unity_plugin use this to specify whether to build one or all target platforms.
string_flag(
    name = "unity-target",
    build_setting_default = "one",
    values = [
        "one",  # Build one target platform (--//reality:unitytarget=one).
        "all",  # Build all target platforms (--//reality:unitytarget=all).
    ],
)

config_setting(
    name = "unity-target-one",
    flag_values = {
        ":unity-target": "one",
    },
)

config_setting(
    name = "unity-target-all",
    flag_values = {
        ":unity-target": "all",
    },
)

refresh_compile_commands(
    name = "refresh_compile_commands",
    targets = [
        "//c8/...",
        "//apps/client/internalqa/...",
        "//reality/app/...",
        "//reality/engine/...",
        "//reality/quality/...",
    ],
)
