load(":android-ndk.bzl", "android_ndk_runtime_libraries")
load("//bzl/android:android-version.bzl", "ANDROID_BUILD_TOOLS_VERSION", "ANDROID_NDK_VERSION")

config_setting(
    name = "arm",
    values = {"cpu": "armeabi"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "armv7",
    values = {"cpu": "armeabi-v7a"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "arm64",
    values = {"cpu": "arm64-v8a"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "mips",
    values = {"cpu": "mips"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "mips64",
    values = {"cpu": "mips64"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "x86",
    values = {"cpu": "x86"},
    visibility = ["//visibility:public"],
)

config_setting(
    name = "x86_64",
    values = {"cpu": "x86_64"},
    visibility = ["//visibility:public"],
)

filegroup(
    name = "manifest",
    srcs = ["manifest.tpl"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "extract-lib-from-apk",
    srcs = ["extract-lib-from-apk.sh"],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "generate-plugin-aar",
    srcs = ["generate-plugin-aar.sh"],
    visibility = ["//visibility:public"],
)

android_library(
    name = "main-activity",
    srcs = ["MainActivity.java"],
    visibility = ["//visibility:public"],
)

android_ndk_runtime_libraries(
    target = "armeabi-v7a",
    visibility = ["//visibility:public"],
)

android_ndk_runtime_libraries(
    target = "arm64-v8a",
    visibility = ["//visibility:public"],
)

android_ndk_runtime_libraries(
    target = "x86",
    visibility = ["//visibility:public"],
)

android_ndk_runtime_libraries(
    target = "x86_64",
    visibility = ["//visibility:public"],
)

genrule(
    name = "cpu-features-h",
    outs = [
        "cpu-features.h",
    ],
    cmd = "cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../ndk/{ver}/sources/android/cpufeatures/cpu-features.h $(RULEDIR)/".format(ver = ANDROID_NDK_VERSION),
    target_compatible_with = [
        "@platforms//os:android",
    ],
    tools = [
        "@androidsdk//:adb",
    ],
)

genrule(
    name = "cpu-features-c",
    outs = [
        "cpu-features.c",
    ],
    cmd = "cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../ndk/{ver}/sources/android/cpufeatures/cpu-features.c $(RULEDIR)/".format(ver = ANDROID_NDK_VERSION),
    target_compatible_with = [
        "@platforms//os:android",
    ],
    tools = [
        "@androidsdk//:adb",
    ],
)

cc_library(
    name = "cpu-features",
    srcs = [
        ":cpu-features-c",
    ],
    hdrs = [
        ":cpu-features-h",
    ],
    strip_include_prefix = ".",
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//visibility:public"],
)

genrule(
    name = "native-app-glue-h",
    outs = [
        "android_native_app_glue.h",
    ],
    cmd = "cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../ndk/{ver}/sources/android/native_app_glue/android_native_app_glue.h $(RULEDIR)/".format(ver = ANDROID_NDK_VERSION),
    target_compatible_with = [
        "@platforms//os:android",
    ],
    tools = [
        "@androidsdk//:adb",
    ],
)

genrule(
    name = "native-app-glue-c",
    outs = [
        "android_native_app_glue.c",
    ],
    cmd = "cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../ndk/{ver}/sources/android/native_app_glue/android_native_app_glue.c $(RULEDIR)/".format(ver = ANDROID_NDK_VERSION),
    target_compatible_with = [
        "@platforms//os:android",
    ],
    tools = [
        "@androidsdk//:adb",
    ],
)

genrule(
    name = "d8-tool",
    outs = [
        "lib/d8.jar",
        "d8",
    ],
    cmd = """
        cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../build-tools/{ver}/lib/d8.jar $(RULEDIR)/lib/
        cp $$(dirname $$(readlink -f $(location @androidsdk//:adb)))/../build-tools/{ver}/d8 $(RULEDIR)/
    """.format(ver = ANDROID_BUILD_TOOLS_VERSION),
    tools = [
        "@androidsdk//:adb",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "native-app-glue",
    srcs = [
        ":native-app-glue-c",
    ],
    hdrs = [
        ":native-app-glue-h",
    ],
    strip_include_prefix = ".",
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":landroid",
    ],
    alwayslink = 1,
)

cc_library(
    name = "llog",
    srcs = [],
    linkopts = [
        "-llog",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "landroid",
    srcs = [],
    linkopts = [
        "-landroid",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "ljnigraphics",
    srcs = [],
    linkopts = [
        "-ljnigraphics",
    ],
    visibility = ["//visibility:public"],
)
