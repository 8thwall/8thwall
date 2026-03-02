load("@the8thwall//bzl/apple:objc.bzl", "nia_objc_library")

WIN32_DEFINES = [
    "_GLFW_WIN32",
]

WIN32_HDRS = [
    "src/win32_platform.h",
    "src/win32_joystick.h",
    "src/wgl_context.h",
    "src/egl_context.h",
    "src/osmesa_context.h",
]

WIN32_SRCS = [
    "src/win32_init.c",
    "src/win32_joystick.c",
    "src/win32_monitor.c",
    "src/win32_time.c",
    "src/win32_thread.c",
    "src/win32_window.c",
    "src/wgl_context.c",
    "src/egl_context.c",
    "src/osmesa_context.c",
]

WIN32_LINKOPTS = [
    "-luser32",
    "-lgdi32",
    "-lshell32",
]

LINUX_DEFINES = [
    "_GLFW_HAS_XF86VM",
    "_GLFW_X11",
]

LINUX_HDRS = [
    "src/x11_platform.h",
    "src/xkb_unicode.h",
    "src/posix_time.h",
    "src/posix_thread.h",
    "src/glx_context.h",
    "src/egl_context.h",
    "src/osmesa_context.h",
    "src/linux_joystick.h",
]

LINUX_SRCS = [
    "src/x11_init.c",
    "src/x11_monitor.c",
    "src/x11_window.c",
    "src/xkb_unicode.c",
    "src/posix_time.c",
    "src/posix_thread.c",
    "src/glx_context.c",
    "src/egl_context.c",
    "src/osmesa_context.c",
    "src/linux_joystick.c",
]

LINUX_LINKOPTS = []

PUBLIC_HDRS = [
    "include/GLFW/glfw3.h",
    "include/GLFW/glfw3native.h",
    "@the8thwall//bzl/glfw:glfw-extra.h",
]

COMMON_HDRS = [
    "src/internal.h",
    "src/mappings.h",
]

COMMON_SRCS = [
    "src/context.c",
    "src/init.c",
    "src/input.c",
    "src/monitor.c",
    "src/vulkan.c",
    "src/window.c",
    "@the8thwall//bzl/glfw:glfw-extra.c",
]

APPLE_DEFINES = [
    "_GLFW_COCOA",
]

APPLE_HDRS = [
    "src/cocoa_platform.h",
    "src/cocoa_joystick.h",
    "src/posix_thread.h",
    "src/nsgl_context.h",
    "src/egl_context.h",
    "src/osmesa_context.h",
    "@the8thwall//bzl/glfw:glfw-extra-apple.h",
]

APPLE_SRCS = [
    "src/cocoa_init.m",
    "src/cocoa_joystick.m",
    "src/cocoa_monitor.m",
    "src/cocoa_window.m",
    "src/cocoa_time.c",
    "src/posix_thread.c",
    "src/nsgl_context.m",
    "src/egl_context.c",
    "src/osmesa_context.c",
    "@the8thwall//bzl/glfw:glfw-extra-apple.m",
]

APPLE_LINKOPTS = []

nia_objc_library(
    name = "glfw-apple",
    srcs = PUBLIC_HDRS + COMMON_HDRS + APPLE_HDRS + COMMON_SRCS + APPLE_SRCS,
    hdrs = [],
    copts = [
        "-D_GLFW_COCOA",
        "-fno-objc-arc",
        "-Iexternal/glfw/include",
    ],
    defines = APPLE_DEFINES,
    linkopts = APPLE_LINKOPTS,
    sdk_frameworks = [
        "Cocoa",
        "QuartzCore",
    ],
)

cc_library(
    name = "glfw",
    srcs = select({
        "@the8thwall//bzl/conditions:windows": COMMON_HDRS + COMMON_SRCS + WIN32_HDRS + WIN32_SRCS,
        "@the8thwall//bzl/conditions:linux": COMMON_HDRS + COMMON_SRCS + LINUX_HDRS + LINUX_SRCS,
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": [],
    }),
    hdrs = PUBLIC_HDRS,
    defines = [
        # Don't include OpenGL headers, clients should depend on
        # //c8/pixels/opengl:gl-headers for GL headers.
        "GLFW_INCLUDE_NONE",
    ] + select({
        "@the8thwall//bzl/conditions:windows": WIN32_DEFINES,
        "@the8thwall//bzl/conditions:linux": LINUX_DEFINES,
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": [],
    }),
    # strip_include_prefix = "include",
    includes = ["include"],
    linkopts = select({
        "@the8thwall//bzl/conditions:windows": WIN32_LINKOPTS,
        "@the8thwall//bzl/conditions:linux": LINUX_LINKOPTS,
        "@the8thwall//bzl/conditions:apple": [],
        "//conditions:default": [],
    }),
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:osx-arm64": [],
        "@the8thwall//bzl/conditions:windows": [],
        "@the8thwall//bzl/conditions:linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
    deps = select({
        "@the8thwall//bzl/conditions:apple": ["glfw-apple"],
        "//conditions:default": [],
    }),
)
