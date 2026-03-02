load("@bazel_skylib//rules:common_settings.bzl", "string_flag")

package(
    default_visibility = ["//visibility:public"],
)

exports_files(["exportOptionsTemplate.plist"])

alias(
    name = "stub",
    actual = select({
        "//bzl/conditions:xcode13": "@xcode13//:stub",
        "//bzl/conditions:xcode14": "@xcode14//:stub",
        "//bzl/conditions:xcode15": "@xcode15//:stub",
        "//bzl/conditions:xcode16": "@xcode16//:stub",
        "//conditions:default": "@xcode15//:stub",
    }),
)

string_flag(
    name = "version",
    build_setting_default = "15",
    values = [
        "13",
        "14",
        "15",
        "16",
    ],
    visibility = ["//visibility:public"],
)

string_flag(
    name = "ios-min-version",
    build_setting_default = "13.0",
    visibility = ["//visibility:public"],
)

string_flag(
    name = "osx-min-version",
    build_setting_default = "11.0",
    visibility = ["//visibility:public"],
)

filegroup(
    name = "xcodebuild",
    srcs = [
        "xcodebuild.sh",
    ],
)

filegroup(
    name = "xcodearchive",
    srcs = [
        "xcodearchive.sh",
    ],
)

filegroup(
    name = "xcodeexport",
    srcs = [
        "xcodeexport.sh",
    ],
)

filegroup(
    name = "xcodebuildsettings",
    srcs = [
        "xcodebuildsettings.sh",
    ],
)

# Flag used when specifying Xcode version used to build apps
string_flag(
    name = "xcode-app-developer-dir",
    build_setting_default = "",
)
