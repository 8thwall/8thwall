licenses(["permissive"])  # Apache 2.0

cc_library(
    name = "openxr",
    hdrs = glob([
        "include/openxr/*.h",
    ]),
    defines = select({
        "@the8thwall//bzl/conditions:android": [
            "XR_OS_ANDROID",
            "XR_USE_PLATFORM_ANDROID",
            "XR_USE_GRAPHICS_API_OPENGL_ES",
        ],
        "@the8thwall//bzl/conditions:linux": ["XR_OS_LINUX"],
        "@the8thwall//bzl/conditions:apple": ["XR_OS_APPLE"],
        "@the8thwall//bzl/conditions:windows": ["XR_OS_WINDOWS"],
        "//conditions:default": [],
    }),
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    deps = [
        "@openxr//src/external/jsoncpp",
    ],
)
