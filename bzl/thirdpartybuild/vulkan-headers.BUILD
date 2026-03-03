licenses(["permissive"])  # Apache-2.0 OR MIT

VULKAN_HDRS = [
    "include/vulkan/vk_icd.h",
    "include/vulkan/vk_layer.h",
    "include/vulkan/vk_platform.h",
    "include/vulkan/vulkan.h",
    "include/vulkan/vulkan.hpp",
    "include/vulkan/vulkan_core.h",
    "include/vulkan/vulkan_screen.h",
    "include/vk_video/vulkan_video_codec_av1std.h",
    "include/vk_video/vulkan_video_codec_av1std_decode.h",
    "include/vk_video/vulkan_video_codec_h264std.h",
    "include/vk_video/vulkan_video_codec_h264std_decode.h",
    "include/vk_video/vulkan_video_codec_h264std_encode.h",
    "include/vk_video/vulkan_video_codec_h265std.h",
    "include/vk_video/vulkan_video_codec_h265std_decode.h",
    "include/vk_video/vulkan_video_codec_h265std_encode.h",
    "include/vk_video/vulkan_video_codecs_common.h",
]

VULKAN_TEXTUAL_HDRS = [
    "include/vulkan/vulkan_android.h",
    "include/vulkan/vulkan_fuchsia.h",
    "include/vulkan/vulkan_ggp.h",
    "include/vulkan/vulkan_ios.h",
    "include/vulkan/vulkan_macos.h",
    "include/vulkan/vulkan_metal.h",
    "include/vulkan/vulkan_vi.h",
    "include/vulkan/vulkan_wayland.h",
    "include/vulkan/vulkan_win32.h",
    "include/vulkan/vulkan_xcb.h",
    "include/vulkan/vulkan_xlib.h",
    "include/vulkan/vulkan_xlib_xrandr.h",
]

cc_library(
    name = "vulkan_headers",
    #  copts = [
    #      "-Wno-redundant-parens",
    #  ],
    hdrs = VULKAN_HDRS,
    defines = select({
        "@the8thwall//bzl/conditions:windows": ["VK_USE_PLATFORM_WIN32_KHR"],
        # "IF_X11":  [ "VK_USE_PLATFORM_XCB_KHR" ],
        # "IF_WAYLAND":  [ "VK_USE_PLATFORM_WAYLAND_KHR" ],
        "@the8thwall//bzl/conditions:android": ["VK_USE_PLATFORM_ANDROID_KHR"],
        # "IF_FUSCHIA":  [ "VK_USE_PLATFORM_FUCHIA_KHR" ],
        "@the8thwall//bzl/conditions:apple": ["VK_USE_PLATFORM_METAL_EXT"],
        # "IF_GGP":  [ "VK_USE_PLATFORM_GGP" ],
        "//conditions:default": [],
    }),
    includes = ["include"],
    textual_hdrs = VULKAN_TEXTUAL_HDRS,
    visibility = ["//visibility:public"],
)

# Like :vulkan_headers but defining VK_NO_PROTOTYPES to disable the
# inclusion of C function prototypes. Useful if dynamically loading
# all symbols via dlopen/etc.
cc_library(
    name = "vulkan_headers_no_prototypes",
    hdrs = VULKAN_HDRS,
    defines = ["VK_NO_PROTOTYPES"],
    includes = ["include"],
    textual_hdrs = VULKAN_TEXTUAL_HDRS,
    visibility = ["//visibility:public"],
)

# Provides a C++-ish interface to Vulkan. A rational set of defines are also
# set and transitively applied to any callers, as well as providing the
# necessary storage as the set of defines leaves symbols undefined otherwise.
cc_library(
    name = "vulkan_hpp",
    srcs =
        select({
            "@the8thwall//bzl/conditions:apple": [],
            "//conditions:default": ["tensorflow/vulkan_hpp_dispatch_loader_dynamic.cc"],
        }),
    hdrs = ["include/vulkan/vulkan.hpp"],
    defines = [
        "VULKAN_HPP_ASSERT=",
        "VULKAN_HPP_DISABLE_IMPLICIT_RESULT_VALUE_CAST",
        "VULKAN_HPP_NO_EXCEPTIONS",
        "VULKAN_HPP_TYPESAFE_CONVERSION",
        "VULKAN_HPP_TYPESAFE_EXPLICIT",
    ] + select({
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": ["VULKAN_HPP_DISPATCH_LOADER_DYNAMIC"],
    }),
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [":vulkan_headers"],
)
