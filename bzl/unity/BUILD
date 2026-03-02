load("@bazel_skylib//rules:common_settings.bzl", "string_flag")
load("//bzl/unity:unity-filegroup.bzl", "unity_filegroup")

exports_files(["manifest-template.json"])

exports_files(["TestAppOptionsTemplate.cs"])

filegroup(
    name = "unity-build",
    srcs = [
        "unity-build.sh",
    ],
    data = [
        ":unity-compile-formatter-py",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "copy-and-sign",
    srcs = [
        "copy-and-sign.sh",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "unity-compile-formatter-py",
    srcs = [
        "unity-compile-formatter.py",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "generate-manifest",
    srcs = [
        "generate-manifest.sh",
    ],
    visibility = ["//visibility:public"],
)

sh_binary(
    name = "generate-upm-package",
    srcs = [
        "generate-upm-package.sh",
    ],
    data = ["//bzl/node:npm"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "editor-scripts",
    srcs = [
        "BazelBuild.cs",
    ],
    visibility = ["//visibility:public"],
)

string_flag(
    name = "notarizing_user",
    build_setting_default = "None",
    visibility = ["//visibility:public"],
)

string_flag(
    name = "notarizing_pass",
    build_setting_default = "None",
    visibility = ["//visibility:public"],
)

# These are Unity headers which code depends on. unity_filegroup is relative to the Contents/
# directory in the appropriate Unity location, i.e. the first file below comes from:
# /Applications/Unity/Hub/Editor/2021.3.12f1/Unity.app/Contents/PluginAPI/IUnityEventQueue.h
# When you use a unity_filegroup you should also create a cc_library which uses this target in hdrs.
unity_filegroup(
    name = "plugin-api-headers",
    srcs = [
        "PluginAPI/IUnityEventQueue.h",
        "PluginAPI/IUnityGraphics.h",
        "PluginAPI/IUnityGraphicsD3D11.h",
        "PluginAPI/IUnityGraphicsD3D12.h",
        "PluginAPI/IUnityGraphicsMetal.h",
        "PluginAPI/IUnityInterface.h",
        "PluginAPI/IUnityRenderingExtensions.h",
        "PluginAPI/IUnityShaderCompilerAccess.h",
    ],
)

cc_library(
    name = "plugin-api",
    hdrs = [":plugin-api-headers"],
    visibility = ["//visibility:public"],
)
