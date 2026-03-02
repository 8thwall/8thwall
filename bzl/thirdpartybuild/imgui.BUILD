load("@the8thwall//bzl/apple:objc.bzl", "nia_objc_library")

cc_library(
    name = "imconfig",
    hdrs = [
        "imconfig.h",
    ],
    deps = [
        "@the8thwall//c8/pixels/opengl:gl-version",
    ],
)

cc_library(
    name = "imgui-hdrs",
    hdrs = [
        "backends/imgui_impl_opengl3.h",
        "backends/imgui_impl_osx.h",
        "imgui.h",
        "imgui_internal.h",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
    ],
    includes = [
        ".",
    ],
    deps = [
        ":imconfig",
    ],
)

cc_library(
    name = "imgui",
    srcs = [
        "backends/imgui_impl_opengl3.cpp",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_tables.cpp",
        "imgui_widgets.cpp",
    ],
    defines = select({
        "@the8thwall//bzl/ios:armv7": [
            "GL_SILENCE_DEPRECATION",
        ],
        "@the8thwall//bzl/ios:arm64": [
            "GL_SILENCE_DEPRECATION",
        ],
        "@the8thwall//bzl/conditions:osx-x86_64": [
            "GL_SILENCE_DEPRECATION",
        ],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":imgui-hdrs",
        ":impl-osx",
    ],
)

nia_objc_library(
    name = "impl-osx",
    srcs = [
        "backends/imgui_impl_osx.mm",
    ],
    sdk_frameworks = [
        "Cocoa",
        "OpenGL",
    ],
    deps = [
        ":imgui-hdrs",
    ],
)

nia_objc_library(
    name = "demo-main-lib",
    srcs = [
        "examples/example_apple_opengl2/main.mm",
        "imgui_demo.cpp",
    ],
    copts = [
        "-Iexternal/imgui/backends",
    ],
    sdk_frameworks = [
        "Cocoa",
        "OpenGL",
    ],
    deps = [
        ":imgui",
    ],
)

# enable this target when imgui is upgraded
#cc_library(
#    name = "imgui-wgpu",
#    srcs = [
#        "backends/imgui_impl_glfw.cpp",
#        "backends/imgui_impl_glfw.h",
#        "backends/imgui_impl_wgpu.cpp",
#        "backends/imgui_impl_wgpu.h",
#        "imgui.cpp",
#        "imgui_draw.cpp",
#        "imgui_tables.cpp",
#        "imgui_widgets.cpp",
#    ],
#    copts = [
#        "-DIMGUI_IMPL_WEBGPU_BACKEND_DAWN",
#    ],
#    visibility = [
#        "//visibility:public",
#    ],
#    deps = [
#        ":imgui-hdrs",
#        "@dawn//:dawn-common",
#        "@glfw",
#    ],
#)

cc_binary(
    name = "demo-main",
    target_compatible_with = [
        "@platforms//os:macos",
    ],
    deps = [
        ":demo-main-lib",
    ],
)
