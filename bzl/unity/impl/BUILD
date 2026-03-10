platform(
    # Default value for the 'original-platform' flag.
    name = "unset-platform",
)

label_flag(
    # Used to pass the original --platforms flag value past the unity platform layer.
    name = "original-platform",
    build_setting_default = ":unset-platform",
)

filegroup(
    name = "unity-csharp-dll",
    srcs = [
        "unity-csharp-dll.sh",
    ],
    visibility = ["//visibility:public"],
)

alias(
    name = "npm_bin",
    actual = select({
        # the node wrapper not picked up correctly on windows
        "//bzl/conditions:windows": "@nodejs_host//:npm_bin",
        "//conditions:default": "//bzl/node:npm",
    }),
    visibility = ["//visibility:public"],
)
