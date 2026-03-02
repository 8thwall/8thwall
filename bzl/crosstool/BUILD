load("@local_config_platform_custom//:constraints.bzl", "REQUIRES_RESPONSE_FILES")
load(":amazonlinux-cc-toolchain.bzl", "amazonlinux_cc_toolchain")
load(":amazonlinux-tool-wrapper.bzl", "amazonlinux_tool_wrapper")
load(":android-cc-toolchain.bzl", "android_toolchain")
load(":android-tool-wrapper.bzl", "android_tool_wrapper")
load(":empty-rust-toolchain.bzl", "empty_rust_toolchain")
load(":v1-linux-cc-toolchain.bzl", "v1_linux_cc_toolchain")
load(":v1-linux-tool-wrapper.bzl", "v1_linux_tool_wrapper")
load(":v2-linux-cc-toolchain.bzl", "v2_linux_cc_toolchain")
load(":v2-linux-tool-wrapper.bzl", "v2_linux_tool_wrapper")
load(":wasm-cc-toolchain.bzl", "wasm_cc_toolchain")
load(":windows-cc-toolchain.bzl", "windows_cc_toolchain")
load(":windows-tool-wrapper.bzl", "windows_tool_wrapper")
load(":xcode-cc-toolchain.bzl", "xcode_cc_toolchain")
load(":xcode-tool-wrapper.bzl", "xcode_tool_wrapper")

# Android SDK minimum version
ANDROID_MIN_VERSION = "24"

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "clang-xcwrapper",
    srcs = ["clang-xcwrapper.sh"],
)

filegroup(
    name = "libtool-xcwrapper",
    srcs = ["libtool-xcwrapper.sh"],
)

xcode_tool_wrapper(
    name = "strip-xcwrapper",
    data = [
        "//bzl/xcode:stub",
        "@workspace-env//:workspace-env",
    ],
    template = "strip-xcwrapper.sh.tpl",
)

xcode_tool_wrapper(
    name = "dsymutil-xcwrapper",
    data = [
        "//bzl/xcode:stub",
        "@workspace-env//:workspace-env",
    ],
    template = "dsymutil-xcwrapper.sh.tpl",
)

filegroup(
    name = "ar-android-wrapper",
    srcs = ["ar-android-wrapper.sh"],
)

filegroup(
    name = "clang-android-wrapper",
    srcs = ["clang-android-wrapper.sh"],
)

android_tool_wrapper(
    name = "strip-android-wrapper",
    prebuilt_dir = "darwin-x86_64",
    template = "strip-android-wrapper.sh.tpl",
)

filegroup(
    name = "tool-wrappers-xcode",
    srcs = [
        ":clang-xcwrapper",
        ":libtool-xcwrapper",
        ":strip-xcwrapper",
        "//bzl/xcode:stub",
        "@workspace-env//:workspace-env",
    ],
)

filegroup(
    name = "clang-windows-wrapper",
    srcs = select({
        # At the moment Batch file are slow and they they make bazel to hang after 100 compilation unit
        # "@platforms//os:windows": ["clang-windows-wrapper.bat"],
        "@platforms//os:windows": ["clang-windows-wrapper-native.exe"],
        "//conditions:default": ["clang-windows-wrapper.sh"],
    }),
)

filegroup(
    name = "ar-windows-wrapper",
    srcs = select({
        # At the moment Batch file are to slow
        # "@platforms//os:windows": ["ar-windows-wrapper.bat"],
        "@platforms//os:windows": ["ar-windows-wrapper-native.exe"],
        "//conditions:default": ["ar-windows-wrapper.sh"],
    }),
)

filegroup(
    name = "ml-windows-wrapper",
    srcs = select({
        # At the moment Batch file are to slow
        # "@platforms//os:windows": ["ml-windows-wrapper.bat"],
        "@platforms//os:windows": ["ml-windows-wrapper-native.exe"],
        "//conditions:default": ["ml-windows-wrapper.sh"],
    }),
)

filegroup(
    name = "strip-windows-wrapper-native",
    srcs = select({
        # At the moment Batch file are to slow
        # "@platforms//os:windows": ["strip-windows-wrapper.bat"],
        "@platforms//os:windows": ["strip-windows-wrapper-native.exe"],
        "//conditions:default": [":strip-windows-wrapper"],
    }),
)

filegroup(
    name = "workspace-env-for-clang-windows-wrapper",
    srcs = select({
        "@platforms//os:windows": ["@workspace-env//:workspace-env-json"],
        "//conditions:default": ["@workspace-env//:workspace-env"],
    }),
)

# TODO: This might be need to be conditional
windows_tool_wrapper(
    name = "strip-windows-wrapper",
    template = "strip-windows-wrapper.sh.tpl",
)

filegroup(
    name = "tool-wrappers-windows",
    srcs = [
        ":ar-windows-wrapper",
        ":clang-windows-wrapper",
        ":ml-windows-wrapper",
        "@local-wine64//:wine64",
        "@msvc//:stub",
        "@workspace-env//:workspace-env",
    ] + select({
        # At the moment Batch file are too slow
        # "@platforms//os:windows": ["clang-windows-wrapper.bat"],
        "@platforms//os:windows": [":strip-windows-wrapper-native"],
        "//conditions:default": [":strip-windows-wrapper"],
    }),
)

filegroup(
    name = "adb",
    srcs = ["@android-sdk//:adb"],
)

filegroup(
    name = "tool-wrappers-android",
    srcs = [
        ":adb",
        ":ar-android-wrapper",
        ":clang-android-wrapper",
        ":strip-android-wrapper",
        "@android-sdk//:stub",
        "@workspace-env//:workspace-env",
    ],
)

filegroup(
    name = "emcc-wrapper",
    srcs = ["emcc-wrapper.sh"],
)

filegroup(
    name = "emar-wrapper",
    srcs = ["emar-wrapper.sh"],
)

filegroup(
    name = "tool-wrappers-wasm",
    srcs = [
        ":emar-wrapper",
        ":emcc-wrapper",
        "@emscripten//:stub",
        "@workspace-env//:workspace-env",
    ],
)

cc_toolchain_suite(
    name = "xcode-cc-toolchain",
    toolchains = {
        "darwin": "cc-compiler-darwin_x86_64",
        "darwin_arm64": "cc-compiler-darwin_arm64",
        "darwin_x86_64": "cc-compiler-darwin_x86_64",
        "darwin_universal": "cc-compiler-darwin_universal",  # Targets a universal binary with x86_64 and arm64.
        "ios_arm64": "cc-compiler-ios_arm64",
        "ios_x86_64": "cc-compiler-ios_x86_64",
        "ios_sim_arm64": "cc-compiler-iossimulator_arm64",
        "arm64-v8a": "cc-compiler-android_arm64-exec-osx",  # Naming follows Bazel conventions for Android ARM64.
        "darwin_dummy": "cc-compiler-darwin_x86_64",  # Required for overcoming analysis of 'objc_*' targets on Linux [Ref: https://github.com/bazelbuild/bazel/issues/16099#issuecomment-1254180342]
    },
)

cc_toolchain_suite(
    name = "windows-cc-toolchain",
    toolchains = {
        "x64_windows": "cc-compiler-x64_windows",
    },
)

cc_toolchain_suite(
    name = "android-cc-toolchain",
    toolchains = {
        "armeabi-v7a": "cc-compiler-android_armv7a-exec-osx",  # Naming follows Bazel conventions for Android EABI ARMv7.
        "arm64-v8a": "cc-compiler-android_arm64-exec-osx",  # Naming follows Bazel conventions for Android ARM64.
        "x86_32": "cc-compiler-android_x86_32-exec-osx",
        "x86_64": "cc-compiler-android_x86_64-exec-osx",
    },
)

cc_toolchain_suite(
    name = "wasm-cc-toolchain",
    toolchains = {
        "wasm32": "cc-compiler-wasm32",
    },
)

xcode_cc_toolchain(
    name = "cc-compiler-darwin_x86_64",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = ["x86_64"],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "macos",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

toolchain(
    name = "cc-toolchain-darwin_x86_64",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:osx",
    ],
    toolchain = ":cc-compiler-darwin_x86_64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

xcode_cc_toolchain(
    name = "cc-compiler-darwin_arm64",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = ["arm64"],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "macos",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

toolchain(
    name = "cc-toolchain-darwin_arm64",
    target_compatible_with = [
        "@platforms//cpu:arm64",
        "@platforms//os:osx",
    ],
    toolchain = ":cc-compiler-darwin_arm64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

constraint_value(
    name = "arm64+x86_64",
    constraint_setting = "@platforms//cpu:cpu",
)

xcode_cc_toolchain(
    name = "cc-compiler-darwin_universal",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = [
        "arm64",
        "x86_64",
    ],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "macos",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

toolchain(
    name = "cc-toolchain-darwin_universal",
    target_compatible_with = [
        ":arm64+x86_64",
        "@platforms//os:osx",
    ],
    toolchain = ":cc-compiler-darwin_universal",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

xcode_cc_toolchain(
    name = "cc-compiler-ios_arm64",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = ["arm64"],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "ios",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

xcode_cc_toolchain(
    name = "cc-compiler-ios_x86_64",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = ["x86_64"],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "ios",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

xcode_cc_toolchain(
    name = "cc-compiler-iossimulator_arm64",
    all_files = ":tool-wrappers-xcode",
    ar_files = ":tool-wrappers-xcode",
    compiler_files = ":tool-wrappers-xcode",
    cpus = ["arm64"],
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-xcode",
    objcopy_files = ":empty",
    platform = "iossimulator",
    strip_files = ":tool-wrappers-xcode",
    supports_param_files = 0,
)

toolchain(
    name = "cc-toolchain-ios_arm64",
    target_compatible_with = [
        "@platforms//cpu:arm64",
        "@platforms//os:ios",
        ":ios-device",
    ],
    toolchain = ":cc-compiler-ios_arm64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

toolchain(
    name = "cc-toolchain-iossimulator_arm64",
    target_compatible_with = [
        "@platforms//cpu:arm64",
        "@platforms//os:ios",
        ":ios-simulator",
    ],
    toolchain = ":cc-compiler-iossimulator_arm64",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

windows_cc_toolchain(
    name = "cc-compiler-x64_windows",
    all_files = ":tool-wrappers-windows",
    ar_files = ":tool-wrappers-windows",
    as_files = ":tool-wrappers-windows",
    compiler_files = ":tool-wrappers-windows",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":empty",
    linker_files = ":tool-wrappers-windows",
    msvc = "@msvc//:stub",
    objcopy_files = ":empty",
    static_runtime_lib = ":empty",
    strip_files = ":tool-wrappers-windows",
    strip_tool = select({
        "@platforms//os:windows": ":strip-windows-wrapper-native",
        "//conditions:default": ":strip-windows-wrapper",
    }),
    # We support param files on Windows, but not on Unix (not really needed there anyway).
    supports_param_files = REQUIRES_RESPONSE_FILES,
    wine64 = "@local-wine64//:wine64",
)

toolchain(
    name = "cc-toolchain-x64_windows-exec-osx",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    toolchain = ":cc-compiler-x64_windows",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

toolchain(
    name = "cc-toolchain-x64_windows-exec-linux",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    toolchain = ":cc-compiler-x64_windows",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

toolchain(
    name = "cc-toolchain-x64_windows-exec-windows",
    exec_compatible_with = [
        "@platforms//os:windows",
    ],
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:windows",
    ],
    toolchain = ":cc-compiler-x64_windows",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

constraint_setting(name = "windows_exec_os_type")

constraint_value(
    name = "windows-exec-os-windows",
    constraint_setting = ":windows_exec_os_type",
)

constraint_value(
    name = "windows-exec-os-unix",
    constraint_setting = ":windows_exec_os_type",
)

###### Android toolchains for OSX host

# The following generates cc-toolchain-android_arm64-exec-osx
android_toolchain(
    name = "android-toolchain-android-arm64-exec-osx",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    exec_os = "osx",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_arm64",
    target = "arm64-v8a",
    target_compatible_with = [
        "@platforms//cpu:arm64",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_armv7a-exec-osx
android_toolchain(
    name = "android-toolchain-android-armv7a-exec-osx",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    exec_os = "osx",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_armv7a",
    target = "armeabi-v7a",
    target_compatible_with = [
        "@platforms//cpu:arm",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_x86_32-exec-osx
android_toolchain(
    name = "android-toolchain-android-x86_32-exec-osx",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    exec_os = "osx",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_x86_32",
    target = "x86",
    target_compatible_with = [
        "@platforms//cpu:x86_32",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_x86_64-exec-osx
android_toolchain(
    name = "android-toolchain-android-x86_64-exec-osx",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:osx",
    ],
    exec_os = "osx",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_x86_64",
    target = "x86_64",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

###### Android toolchains for Linux host

# The following generates cc-toolchain-android_arm64-exec-linux
android_toolchain(
    name = "android-toolchain-android-arm64-exec-linux",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    exec_os = "linux",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_arm64",
    target = "arm64-v8a",
    target_compatible_with = [
        "@platforms//cpu:arm64",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_armv7a-exec-linux
android_toolchain(
    name = "android-toolchain-android-armv7a-exec-linux",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    exec_os = "linux",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_armv7a",
    target = "armeabi-v7a",
    target_compatible_with = [
        "@platforms//cpu:arm",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_x86_32-exec-linux
android_toolchain(
    name = "android-toolchain-android-x86_32-exec-linux",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    exec_os = "linux",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_x86_32",
    target = "x86",
    target_compatible_with = [
        "@platforms//cpu:x86_32",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

# The following generates cc-toolchain-android_x86_64-exec-linux
android_toolchain(
    name = "android-toolchain-android-x86_64-exec-linux",
    adb = ":adb",
    exec_compatible_with = [
        "@platforms//os:linux",
    ],
    exec_os = "linux",
    min_version = ANDROID_MIN_VERSION,
    platform = "android_x86_64",
    target = "x86_64",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:android",
    ],
    tools = ":tool-wrappers-android",
)

constraint_setting(name = "wasm-setting")

constraint_value(
    name = "wasm",
    constraint_setting = ":wasm-setting",
)

wasm_cc_toolchain(
    name = "cc-compiler-wasm32",
    all_files = ":tool-wrappers-wasm",
    ar_files = ":tool-wrappers-wasm",
    compiler_files = ":tool-wrappers-wasm",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":empty",
    linker_files = ":tool-wrappers-wasm",
    objcopy_files = ":empty",
    static_runtime_lib = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain = "@emscripten//:stub",
)

toolchain(
    name = "cc-toolchain-wasm32",
    target_compatible_with = [
        "@platforms//cpu:wasm32",
    ],
    toolchain = ":cc-compiler-wasm32",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

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
        ":arm64+x86_64",
    ],
)

platform(
    name = "ios_arm64",
    constraint_values = [
        "@platforms//os:ios",
        "@platforms//cpu:arm64",
        ":ios-device",
    ],
)

platform(
    name = "ios_sim_arm64",
    constraint_values = [
        "@platforms//os:ios",
        "@platforms//cpu:arm64",
        ":ios-simulator",
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
    name = "windows_x86_64_exec_unix",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
        ":windows-exec-os-unix",
    ],
)

platform(
    name = "windows_x86_64_exec_windows",
    constraint_values = [
        "@platforms//os:windows",
        "@platforms//cpu:x86_64",
        ":windows-exec-os-windows",
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

# Adding new contraint type to support iossimulator

constraint_setting(name = "ios_host_type")

constraint_value(
    name = "ios-simulator",
    constraint_setting = ":ios_host_type",
)

constraint_value(
    name = "ios-device",
    constraint_setting = ":ios_host_type",
)

# Linux targets
constraint_setting(
    name = "linux-flavor-setting",
    default_constraint_value = ":v1-linux",
)

constraint_value(
    name = "amazonlinux",
    constraint_setting = ":linux-flavor-setting",
)

constraint_value(
    name = "v1-linux",
    constraint_setting = ":linux-flavor-setting",
)

constraint_value(
    name = "v2-linux",
    constraint_setting = ":linux-flavor-setting",
)

constraint_setting(
    name = "gpu",
    default_constraint_value = ":gpu-none",
)

constraint_value(
    name = "gpu-none",
    constraint_setting = ":gpu",
)

constraint_value(
    name = "gpu-nvidia",
    constraint_setting = ":gpu",
)

# Rules for building for Amazon linux from Mac
filegroup(
    name = "clang-amazonlinux-wrapper",
    srcs = ["clang-amazonlinux-wrapper.sh"],
)

filegroup(
    name = "ar-amazonlinux-wrapper",
    srcs = ["ar-linux-wrapper.sh"],
)

amazonlinux_tool_wrapper(
    name = "strip-amazonlinux-wrapper",
    template = "strip-linux-wrapper.sh.tpl",
)

filegroup(
    name = "tool-wrappers-amazonlinux",
    srcs = [
        ":ar-amazonlinux-wrapper",
        ":clang-amazonlinux-wrapper",
        ":strip-amazonlinux-wrapper",
        "@amazonlinux//:stub",
        "@workspace-env//:workspace-env",
    ],
)

cc_toolchain_suite(
    name = "amazonlinux-cc-toolchain",
    toolchains = {
        "x86_64": "cc-compiler-amazonlinux",
    },
)

amazonlinux_cc_toolchain(
    name = "cc-compiler-amazonlinux",
    all_files = ":tool-wrappers-amazonlinux",
    ar_files = ":tool-wrappers-amazonlinux",
    compiler_files = ":tool-wrappers-amazonlinux",
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-amazonlinux",
    # this is the c and c++ stdlib for the target we're building for.
    linux = "@amazonlinux//:stub",
    objcopy_files = ":empty",
    supports_param_files = 0,
    tools = ":tool-wrappers-amazonlinux",
)

toolchain(
    name = "cc-toolchain-amazonlinux",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
        ":amazonlinux",
    ],
    toolchain = ":cc-compiler-amazonlinux",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

###############################################################################
# Rules for building for V1 linux
# https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/co6yA1pW
filegroup(
    name = "clang-v1-linux-wrapper",
    srcs = ["clang-v1-linux-wrapper.sh"],
)

filegroup(
    name = "ar-v1-linux-wrapper",
    srcs = ["ar-linux-wrapper.sh"],
)

v1_linux_tool_wrapper(
    name = "strip-v1-linux-wrapper",
    template = "strip-linux-wrapper.sh.tpl",
)

filegroup(
    name = "tool-wrappers-v1-linux",
    srcs = [
        ":ar-v1-linux-wrapper",
        ":clang-v1-linux-wrapper",
        ":strip-v1-linux-wrapper",
        "@v1-linux//:stub",
        "@workspace-env//:workspace-env",
    ] + select({
        "@the8thwall//bzl/conditions:cuda-hermetic": ["@v1-cuda-triplet//:stub"],
        "@the8thwall//bzl/conditions:cuda-system": ["@v1-cuda-triplet//:stub"],
        "//conditions:default": [],
    }),
)

cc_toolchain_suite(
    name = "v1-linux-cc-toolchain",
    toolchains = {
        "x86_64": "cc-compiler-v1-linux",
    },
)

# Create a dictionary that maps all //bzl/cpu-microarch targets to itself so it can be used in a select statement.
targetable_micro_arch_dict = {x: x for x in [
    "@the8thwall//bzl/cpu-microarch:x86-64",
    "@the8thwall//bzl/cpu-microarch:cascadelake",
    "@the8thwall//bzl/cpu-microarch:native",
    "@the8thwall//bzl/cpu-microarch:skylake",
    "@the8thwall//bzl/cpu-microarch:skylake-avx512",
    "@the8thwall//bzl/cpu-microarch:not-specified",
]}

v1_linux_cc_toolchain(
    name = "cc-compiler-v1-linux",
    all_files = ":tool-wrappers-v1-linux",
    ar_files = ":tool-wrappers-v1-linux",
    compiler_files = ":tool-wrappers-v1-linux",
    cpu_microarch = select(targetable_micro_arch_dict),
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-v1-linux",
    # this is the c and c++ stdlib for the target we're building for.
    linux = "@v1-linux//:stub",
    objcopy_files = ":empty",
    supports_param_files = 0,
    tools = ":tool-wrappers-v1-linux",
)

toolchain(
    name = "cc-toolchain-v1-linux",
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":v1-linux",
    ],
    toolchain = ":cc-compiler-v1-linux",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

###############################################################################
# Rules for building for V2 linux from Mac (experimental)
# https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/X25vkYhE
filegroup(
    name = "clang-v2-linux-wrapper",
    srcs = ["clang-v2-linux-wrapper.sh"],
)

filegroup(
    name = "ar-v2-linux-wrapper",
    srcs = ["ar-linux-wrapper.sh"],
)

v2_linux_tool_wrapper(
    name = "strip-v2-linux-wrapper",
    template = "strip-linux-wrapper.sh.tpl",
)

filegroup(
    name = "tool-wrappers-v2-linux",
    srcs = [
        ":ar-v2-linux-wrapper",
        ":clang-v2-linux-wrapper",
        ":strip-v2-linux-wrapper",
        "@v2-linux//:stub",
        "@workspace-env//:workspace-env",
    ] + select({
        "@the8thwall//bzl/conditions:cuda-hermetic": ["@v1-cuda-triplet//:stub"],
        "@the8thwall//bzl/conditions:cuda-system": ["@v1-cuda-triplet//:stub"],
        "//conditions:default": [],
    }),
)

cc_toolchain_suite(
    name = "v2-linux-cc-toolchain",
    toolchains = {
        "x86_64": "cc-compiler-v2-linux",
    },
)

v2_linux_cc_toolchain(
    name = "cc-compiler-v2-linux",
    all_files = ":tool-wrappers-v2-linux",
    ar_files = ":tool-wrappers-v2-linux",
    compiler_files = ":tool-wrappers-v2-linux",
    cpu_microarch = select(targetable_micro_arch_dict),
    dwp_files = ":empty",
    linker_files = ":tool-wrappers-v2-linux",
    # this is the c and c++ stdlib for the target we're building for.
    linux = "@v2-linux//:stub",
    objcopy_files = ":empty",
    supports_param_files = 0,
    tools = ":tool-wrappers-v2-linux",
)

toolchain(
    name = "cc-toolchain-v2-linux",
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
        ":v2-linux",
    ],
    toolchain = ":cc-compiler-v2-linux",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

empty_rust_toolchain(
    name = "empty-rust-toolchain-inner",
)

toolchain(
    name = "empty-rust-toolchain",
    toolchain = ":empty-rust-toolchain-inner",
    toolchain_type = "@rules_rust//rust:toolchain_type",
)

exports_files(
    ["rustc-wrapper.sh"],
    visibility = ["//visibility:public"],
)

###############################################################################
# Dynamic ldd resolution according to the target
# https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/l/cp/X25vkYhE

alias(
    name = "hermetic_ldd",
    actual = select({
        "//bzl/conditions:v1-linux": "@v1-linux//:hermetic_ldd",
        "//bzl/conditions:v2-linux": "@v2-linux//:hermetic_ldd",
        "//conditions:default": "hermetic_toolchain_not_supported",
    }),
)

alias(
    name = "hermetic_objdump",
    actual = select({
        "//bzl/conditions:v1-linux": "@v1-linux//:hermetic_objdump",
        "//bzl/conditions:v2-linux": "@v2-linux//:hermetic_objdump",
        "//conditions:default": "hermetic_toolchain_not_supported",
    }),
)

alias(
    name = "hermetic_libgcc_s",
    actual = select({
        "//bzl/conditions:v1-linux": "@v1-linux//:hermetic_libgcc_s",
        "//bzl/conditions:v2-linux": "@v2-linux//:hermetic_libgcc_s",
        "//conditions:default": "hermetic_toolchain_not_supported",
    }),
)

alias(
    name = "hermetic_libstdc++",
    actual = select({
        "//bzl/conditions:v1-linux": "@v1-linux//:hermetic_libstdc++",
        "//bzl/conditions:v2-linux": "@v2-linux//:hermetic_libstdc++",
        "//conditions:default": "hermetic_toolchain_not_supported",
    }),
)
