load("@bazel_skylib//lib:selects.bzl", "selects")
load("@local_config_platform_custom//:constraints.bzl", "HOST_CONSTRAINTS")

package(
    default_visibility = ["//visibility:public"],
)

exports_files(["test_conditions.sh"])

selects.config_setting_group(
    name = "local_platform",
    match_all = HOST_CONSTRAINTS,
)

config_setting(
    name = "android",
    constraint_values = [
        "@platforms//os:android",
    ],
)

config_setting(
    name = "android-arm",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm",
    ],
)

alias(
    name = "android-arm32",
    actual = ":android-arm",
)

config_setting(
    name = "android-arm64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm64",
    ],
)

config_setting(
    name = "android-x86_32",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:x86_32",
    ],
)

config_setting(
    name = "android-x86_64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:x86_64",
    ],
)

alias(
    name = "magicleap2",
    actual = ":android-x86_64",
)

config_setting(
    name = "linux",
    constraint_values = ["@platforms//os:linux"],
)

config_setting(
    name = "linux-x86_64",
    constraint_values = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

v1_linux_constraint_values = [
    "@platforms//os:linux",
    "@platforms//cpu:x86_64",
    "//bzl/crosstool:v1-linux",
]

selects.config_setting_group(
    name = "v1-linux",
    match_all = v1_linux_constraint_values,
)

selects.config_setting_group(
    name = "v1-linux-gpu-none",
    match_all = v1_linux_constraint_values + [
        "//bzl/crosstool:gpu-none",
    ],
)

selects.config_setting_group(
    name = "v1-linux-cuda-system",
    match_all = v1_linux_constraint_values + [
        "//bzl/crosstool:gpu-nvidia",
    ],
)

selects.config_setting_group(
    name = "v2-linux",
    match_all = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        "//bzl/crosstool:v2-linux",
    ],
)

selects.config_setting_group(
    name = "android-or-linux",
    match_any = [
        ":android",
        ":linux",
    ],
)

config_setting(
    name = "windows",
    constraint_values = ["@platforms//os:windows"],
)

config_setting(
    name = "windows-x86_32",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_32",
    ],
)

config_setting(
    name = "windows-x86_64",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "osx",
    constraint_values = ["@platforms//os:osx"],
)

alias(
    name = "macos",
    actual = ":osx",
)

config_setting(
    name = "osx-x86_64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:x86_64",
    ],
)

config_setting(
    name = "osx-arm64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:arm64",
    ],
)

config_setting(
    name = "ios-arm64",
    constraint_values = [
        "@platforms//os:ios",
        "@platforms//cpu:arm64",
    ],
)

config_setting(
    name = "osx-arm64+x86_64",
    constraint_values = [
        "@platforms//os:osx",
        "//bzl/crosstool:arm64+x86_64",
    ],
)

config_setting(
    name = "ios",
    constraint_values = ["@platforms//os:ios"],
)

selects.config_setting_group(
    name = "apple",
    match_any = [
        ":osx",
        ":ios",
    ],
)

config_setting(
    name = "arm",
    constraint_values = ["@platforms//cpu:arm"],
)

config_setting(
    name = "arm64",
    constraint_values = ["@platforms//cpu:arm64"],
)

selects.config_setting_group(
    name = "arm-any",
    match_any = [
        ":arm",
        ":arm64",
    ],
)

config_setting(
    name = "x86_32",
    constraint_values = ["@platforms//cpu:x86_32"],
)

config_setting(
    name = "x86_64",
    constraint_values = ["@platforms//cpu:x86_64"],
)

selects.config_setting_group(
    name = "x86-any",
    match_any = [
        ":x86_32",
        ":x86_64",
    ],
)

selects.config_setting_group(
    name = "32bit",
    match_any = [
        ":x86_32",
        ":arm",
        "@platforms//cpu:wasm32",
    ],
)

selects.config_setting_group(
    name = "64bit",
    match_any = [
        ":x86_64",
        ":arm64",
        "//bzl/crosstool:arm64+x86_64",
        "@platforms//cpu:wasm64",
    ],
)

selects.config_setting_group(
    name = "apple32",
    match_all = [
        ":32bit",
        ":apple",
    ],
)

selects.config_setting_group(
    name = "apple64",
    match_all = [
        ":64bit",
        ":apple",
    ],
)

selects.config_setting_group(
    name = "android32",
    match_all = [
        ":32bit",
        ":android",
    ],
)

selects.config_setting_group(
    name = "android64",
    match_all = [
        ":64bit",
        ":android",
    ],
)

selects.config_setting_group(
    name = "windows32",
    match_all = [
        ":32bit",
        ":windows",
    ],
)

selects.config_setting_group(
    name = "windows64",
    match_all = [
        ":64bit",
        ":windows",
    ],
)

config_setting(
    name = "wasm32",
    constraint_values = ["@platforms//cpu:wasm32"],
)

config_setting(
    name = "wasm64",
    constraint_values = ["@platforms//cpu:wasm64"],
)

config_setting(
    name = "wasm",
    constraint_values = ["//bzl/crosstool:wasm"],
)

config_setting(
    name = "xcode13",
    flag_values = {
        "//bzl/xcode:version": "13",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "xcode14",
    flag_values = {
        "//bzl/xcode:version": "14",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "xcode15",
    flag_values = {
        "//bzl/xcode:version": "15",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "xcode16",
    flag_values = {
        "//bzl/xcode:version": "16",
    },
    visibility = ["//visibility:public"],
)

selects.config_setting_group(
    name = "mobile",
    match_any = [
        ":ios",
        ":android",
        ":android-arm64",
        ":android-arm",
    ],
)

config_setting(
    name = "simd",
    values = {
        "features": "simd",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "wasm-pthread",
    constraint_values = ["//bzl/crosstool:wasm"],
    values = {
        "features": "pthread",
    },
    visibility = ["//visibility:public"],
)

#######################################
## Cuda system conditions
## (those are also used by *-linux-tool-wrapper.bzl)

# No cuda used
selects.config_setting_group(
    name = "cuda-none",
    match_all = [
        "//bzl/gpu:cuda-none",
    ],
)

# Using cuda pre-installed on the system
# Cuda needs to be available at /usr/local/cuda and for there it will
# be linked in the sandbox compilations
selects.config_setting_group(
    name = "cuda-system",
    match_all = [
        "//bzl/gpu:cuda-system",
    ],
)

# Using the cuda hermetic ones
selects.config_setting_group(
    name = "cuda-hermetic",
    match_all = [
        "//bzl/gpu:cuda-hermetic",
    ],
)

selects.config_setting_group(
    name = "cuda-incompatible",
    match_any = [
        ":windows",
        ":apple",
        ":cuda-none",
    ],
)

## Build config conditions
# Activates for compilation mode "-c opt" (release build)
config_setting(
    name = "config_setting_opt",
    values = {"compilation_mode": "opt"},
)

# Activates for compilation mode "-c fastbuild"
config_setting(
    name = "config_setting_fastbuild",
    values = {"compilation_mode": "fastbuild"},
)

# Activates for compilation mode "-c dbg"
config_setting(
    name = "config_setting_debug",
    values = {"compilation_mode": "dbg"},
)

##########################################################
# Conditions can become quite complex to follow at times.
# This is a testing playground target which allows
# to test the actual conditions resultant
# in a bazel run.
#
# Uncomment and use in case of need
#

# sh_binary(
#     name = "test_conditions",
#     srcs = ["test_conditions.sh"],
#     args = select({
#         ":v1-linux-gpu-none": ["v1-linux-gpu-none"],
#         "//conditions:default": ["no linux"],
#     }) + select({
#         ":android": ["android"],
#         "//conditions:default": ["no android"],
#     }) + select({
#         ":apple": ["apple"],
#         "//conditions:default": ["no apple"],
#     }),
# )
