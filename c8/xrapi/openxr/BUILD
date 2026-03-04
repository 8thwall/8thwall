load("//bzl/zip:unzip.bzl", "unzip")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "graphics",
    srcs = [
        "graphics.cc",
    ],
    hdrs = [
        "graphics.h",
    ],
    deps = [
        ":helper",
        ":openxr",
    ],
)

cc_library(
    name = "meta-foveation",
    srcs = [
        "meta-foveation.cc",
    ],
    hdrs = [
        "meta-foveation.h",
    ],
    deps = [
        ":helper",
        ":openxr",
        "//c8:vector",
    ],
)

cc_library(
    name = "display-refresh-rate-fb",
    srcs = [
        "display-refresh-rate-fb.cc",
    ],
    hdrs = [
        "display-refresh-rate-fb.h",
    ],
    deps = [
        ":helper",
        ":openxr",
        "//c8:vector",
    ],
)

cc_library(
    name = "meta-virtual-keyboard",
    srcs = [
        "meta-virtual-keyboard.cc",
    ],
    hdrs = [
        "meta-virtual-keyboard.h",
    ],
    deps = [
        ":helper",
        ":openxr",
    ],
)

cc_library(
    name = "helper",
    hdrs = [
        "helper.h",
    ],
    deps = [
        ":openxr",
        "//c8:c8-log",
    ],
)

cc_library(
    name = "openxr",
    hdrs = ["openxr.h"],
    deps = [
        ":openxr-lib",
        "//c8/pixels/opengl:gl-headers",
        "@openxr",
    ],
)

cc_import(
    name = "openxr-lib",
    shared_library = ":openxr-lib-so",
    target_compatible_with = [
        "@platforms//os:android",
    ],
    visibility = ["//visibility:public"],
)

unzip(
    name = "openxr-lib-so",
    srcs = [
        "openxr_loader_for_android-1.1.38.aar",
    ],
    outs =
        select({
            "//bzl/conditions:android-arm64": [
                "prefab/modules/openxr_loader/libs/android.arm64-v8a/libopenxr_loader.so",
            ],
            "//bzl/conditions:android-arm": [
                "prefab/modules/openxr_loader/libs/android.armeabi-v7a/libopenxr_loader.so",
            ],
            "//bzl/conditions:android-x86_32": [
                "prefab/modules/openxr_loader/libs/android.x86/libopenxr_loader.so",
            ],
            "//bzl/conditions:android-x86_64": [
                "prefab/modules/openxr_loader/libs/android.x86_64/libopenxr_loader.so",
            ],
            "//conditions:default": [],
        }),
    target_compatible_with = [
        "@platforms//os:android",
    ],
)

cc_library(
    name = "gl-graphics",
    srcs = [
        "gl-graphics.cc",
    ],
    hdrs = [
        "gl-graphics.h",
    ],
    deps = [
        ":gl-impl",
        ":gl-texture-auto-restore",
        ":helper",
        ":openxr",
        "//c8:c8-log",
        "//c8:vector",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-headers",
    ],
)

cc_library(
    name = "gl-contexts",
    srcs = [
        "gl-contexts.cc",
    ],
    hdrs = [
        "gl-contexts.h",
    ],
    deps = [
        ":gl-impl",
        ":openxr",
        "//c8:vector",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-extensions",
        "//c8/pixels/opengl:gl-headers",
    ],
)

cc_library(
    name = "gl-texture-conversion",
    srcs = [
        "gl-texture-conversion.cc",
    ],
    hdrs = [
        "gl-texture-conversion.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-error",
        "//c8/pixels/opengl:gl-extensions",
        "//c8/pixels/opengl:gl-framebuffer",
        "//c8/pixels/opengl:gl-headers",
        "//c8/pixels/opengl:gl-program",
        "//c8/pixels/opengl:gl-quad",
        "//c8/pixels/opengl:gl-texture",
        "//c8/pixels/opengl:gl-version",
        "//c8/pixels/opengl:gl-vertex-array",
    ],
)

cc_test(
    name = "gl-texture-conversion-test",
    size = "small",
    srcs = [
        "gl-texture-conversion-test.cc",
    ],
    deps = [
        ":gl-texture-conversion",
        "//c8:c8-log",
        "//c8:vector",
        "//c8/pixels/opengl:gl-texture",
        "//c8/pixels/opengl:offscreen-gl-context",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "gl-texture-auto-restore",
    hdrs = [
        "gl-texture-auto-restore.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8/command:command-buffer",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-headers",
    ],
)

cc_library(
    name = "gl-impl",
    srcs = [
        "gl-impl.cc",
    ],
    hdrs = [
        "gl-impl.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8/command:command-buffer",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-headers",
    ],
)

cc_library(
    name = "gl-motion-estimation",
    hdrs = [
        "gl-motion-estimation.h",
    ],
    deps = [
        "//c8:c8-log",
        "//c8/pixels/opengl:client-gl",
        "//c8/pixels/opengl:gl-error",
        "//c8/pixels/opengl:gl-headers",
    ],
)

cc_library(
    name = "ext-performance-settings",
    srcs = [
        "ext-performance-settings.cc",
    ],
    hdrs = [
        "ext-performance-settings.h",
    ],
    deps = [
        ":helper",
        ":openxr",
        "//c8:vector",
    ],
)
