load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

filegroup(
    name = "metal-build",
    srcs = [
        "metal-build.sh",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "metallib-build",
    srcs = [
        "metallib-build.sh",
    ],
    visibility = ["//visibility:public"],
)

###############################################################################
# String flag for enabling CUDA support
# This has effect only with --platforms=//bzl:v*-linux
string_flag(
    name = "cuda-support",
    build_setting_default = "none",
    values = [
        "none",  # Disable cuda support
        "system",  # Use Cuda installed on the system (tested in nvidia-docker2)
        "hermetic",  # Use Cuda-triplet hermetic installation
        # (the cuda-triplet used, is included in tool-wrappers file group)
    ],
)

config_setting(
    name = "cuda-none",
    flag_values = {
        ":cuda-support": "none",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "cuda-system",
    flag_values = {
        ":cuda-support": "system",
    },
    visibility = ["//visibility:public"],
)

config_setting(
    name = "cuda-hermetic",
    flag_values = {
        ":cuda-support": "hermetic",
    },
    visibility = ["//visibility:public"],
)

filegroup(
    name = "verify_cuda_env",
    srcs = [
        "verify_cuda_env.sh",
    ],
    visibility = ["//visibility:public"],
)
