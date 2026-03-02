package(default_visibility = ["//visibility:public"])

platform(
    name = "osx_x86_64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "osx_arm64",
    constraint_values = [
        "@platforms//os:osx",
        "@platforms//cpu:arm64",
    ],
)

platform(
    name = "osx_universal",
    constraint_values = [
        "@platforms//os:osx",
        "//bzl/crosstool:arm64+x86_64",
    ],
)

platform(
    name = "ios_arm64",
    constraint_values = [
        "@platforms//os:ios",
        "@platforms//cpu:arm64",
        "//bzl/crosstool:ios-device",
        "@build_bazel_apple_support//constraints:device",
    ],
)

platform(
    name = "ios_sim_arm64",
    constraint_values = [
        "@platforms//os:ios",
        "@platforms//cpu:arm64",
        "//bzl/crosstool:ios-simulator",
        "@build_bazel_apple_support//constraints:simulator",
    ],
)

platform(
    name = "windows_x86_64",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "android_arm64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm64",
    ],
)

platform(
    name = "android_armv7a",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:arm",
    ],
)

platform(
    name = "android_x86_32",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:x86_32",
    ],
)

platform(
    name = "android_x86_64",
    constraint_values = [
        "@platforms//os:android",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "wasm32",
    constraint_values = [
        "@platforms//cpu:wasm32",
        "//bzl/crosstool:wasm",
    ],
)

platform(
    name = "amazonlinux",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:amazonlinux",
    ],
)

platform(
    name = "v1-linux",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
    ],
)

platform(
    name = "v1-linux-gpu-none",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/crosstool:gpu-none",
    ],
)

platform(
    name = "v1-linux-cuda-system",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:native",
        "//bzl/crosstool:gpu-nvidia",
    ],
)

platform(
    name = "v1-linux-skylake-avx512-cuda-system",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:skylake-avx512",
        "//bzl/crosstool:gpu-nvidia",
    ],
)

platform(
    name = "v1-linux_x86_64_x86-64",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:x86-64",
    ],
)

# Linux platform target for intel cascadelake micro architecture.
platform(
    name = "v1-linux_x86_64_cascadelake",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:cascadelake",
    ],
)

# Linux platform target for host native micro architecture.
platform(
    name = "v1-linux_x86_64_native",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:native",
    ],
)

# Linux platform target for intel skylake micro architecture.
platform(
    name = "v1-linux_x86_64_skylake",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:skylake",
    ],
)

# Linux platform target for intel skylake micro architecture with avx512.
platform(
    name = "v1-linux_x86_64_skylake-avx512",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v1-linux",
        "//bzl/cpu-microarch:skylake-avx512",
    ],
)

platform(
    name = "v2-linux",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
    ],
)

platform(
    name = "v2-linux_x86_64_x86-64",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
        "//bzl/cpu-microarch:x86-64",
    ],
)

# v2 linux platform target for intel cascadelake micro architecture.
platform(
    name = "v2-linux_x86_64_cascadelake",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
        "//bzl/cpu-microarch:cascadelake",
    ],
)

# v2 linux platform target for host native micro architecture.
platform(
    name = "v2-linux_x86_64_native",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
        "//bzl/cpu-microarch:native",
    ],
)

# v2 linux platform target for intel skylake micro architecture.
platform(
    name = "v2-linux_x86_64_skylake",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
        "//bzl/cpu-microarch:skylake",
    ],
)

# v2 linux platform target for intel skylake micro architecture with avx512.
platform(
    name = "v2-linux_x86_64_skylake-avx512",
    constraint_values = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        "//bzl/crosstool:v2-linux",
        "//bzl/cpu-microarch:skylake-avx512",
    ],
)

platform(
    name = "wasm64",
    constraint_values = [
        "@platforms//cpu:wasm64",
        "//bzl/crosstool:wasm",
    ],
)

# Bazel mobile-install uses the bazel platform name as the ABI, which must match one of Android's abis [x86, x86_64, arm64-v8a, armeabi-v7a]
# See https://github.com/bazelbuild/bazel/issues/15837 filed about this issue.
alias(
    name = "arm64-v8a",
    actual = ":android_arm64",
)

alias(
    name = "armeabi-v7a",
    actual = ":android_armv7a",
)

alias(
    name = "android_arm32",
    actual = ":android_armv7a",
)

# Bazel alias for contemporary Apple names
alias(
    name = "macos_arm64",
    actual = ":osx_arm64",
)

alias(
    name = "macos_x86_64",
    actual = ":osx_x86_64",
)
