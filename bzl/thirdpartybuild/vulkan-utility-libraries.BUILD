licenses(["permissive"])  # Apache-2.0 OR MIT

COMMON_COPTS = [
    "-DVK_ENABLE_BETA_EXTENSIONS",
] + select({
    "@the8thwall//bzl/conditions:android": [
        "-DVK_USE_PLATFORM_ANDROID_KHR",
    ],
    "@the8thwall//bzl/conditions:ios": [
        "-DVK_USE_PLATFORM_METAL_EXT",
        "-DVK_USE_PLATFORM_IOS_MVK",
    ],
    "@the8thwall//bzl/conditions:osx": [
        "-DVK_USE_PLATFORM_METAL_EXT",
        "-DVK_USE_PLATFORM_MACOS_MVK",
    ],
    "@the8thwall//bzl/conditions:linux": ["-DVK_USE_PLATFORM_XLIB_KHR"],
    "@the8thwall//bzl/conditions:windows": [
        "-DVK_USE_PLATFORM_WIN32_KHR",
    ],
    "//conditions:default": [],
})

VULKAN_UTILITY_HDRS = [
    "include/vulkan/vk_enum_string_helper.h",
    "include/vulkan/utility/vk_concurrent_unordered_map.hpp",
    "include/vulkan/utility/vk_dispatch_table.h",
    "include/vulkan/utility/vk_format_utils.h",
    "include/vulkan/utility/vk_small_containers.hpp",
    "include/vulkan/utility/vk_sparse_range_map.hpp",
    "include/vulkan/utility/vk_struct_helper.hpp",
]

cc_library(
    name = "vulkan_utility_headers",
    hdrs = VULKAN_UTILITY_HDRS,
    copts = COMMON_COPTS,
    includes = ["include"],
    visibility = ["//visibility:public"],
)

# ref src/layer/CMakeLists.txt
cc_library(
    name = "vulkan_layer_settings",
    srcs = [
        "src/layer/vk_layer_settings.cpp",
        "src/layer/vk_layer_settings_helper.cpp",
        "src/layer/layer_settings_manager.cpp",
        "src/layer/layer_settings_manager.hpp",
        "src/layer/layer_settings_util.cpp",
        "src/layer/layer_settings_util.hpp",
    ],
    hdrs = [
        "include/vulkan/layer/vk_layer_settings.h",
        "include/vulkan/layer/vk_layer_settings.hpp",
    ],
    copts = COMMON_COPTS,
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [
        "@vulkan_headers//:vulkan_headers",
        ":vulkan_utility_headers",
    ],
)

# ref src/vulkan/CMakeLists.txt
cc_library(
    name = "vulkan_safe_struct",
    srcs = [
        "src/vulkan/vk_safe_struct_core.cpp",
        "src/vulkan/vk_safe_struct_ext.cpp",
        "src/vulkan/vk_safe_struct_khr.cpp",
        "src/vulkan/vk_safe_struct_manual.cpp",
        "src/vulkan/vk_safe_struct_utils.cpp",
        "src/vulkan/vk_safe_struct_vendor.cpp",
    ],
    hdrs = [
        "include/vulkan/utility/vk_safe_struct.hpp",
        "include/vulkan/utility/vk_safe_struct_utils.hpp",
    ],
    copts = COMMON_COPTS,
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = [
        "@vulkan_headers//:vulkan_headers",
        ":vulkan_utility_headers",
    ],
)
