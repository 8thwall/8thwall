# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "stub",
    srcs = [],
)

filegroup(
    name = "all-files",
    srcs = glob(["**/*"]),
)

cc_library(
    name = "opengl",
    hdrs = glob([
        "**/include/EGL/*.h",
        "**/include/GL/*.h",
        "**/include/GLES/*.h",
        "**/include/GLES2/*.h",
        "**/include/GLES3/*.h",
        "**/include/KHR/*.h",
        "**/include/X11/*.h",
        "**/include/EGL/**/*.h",
        "**/include/GL/**/*.h",
        "**/include/GLES/**/*.h",
        "**/include/GLES2/**/*.h",
        "**/include/GLES3/**/*.h",
        "**/include/KHR/**/*.h",
        "**/include/X11/**/*.h",
    ]),
    linkopts = select({
        "@the8thwall//bzl/conditions:v1-linux": [
            "-lGLESv2",
            "-lGLdispatch",
        ],
        "//conditions:default": [],
    }),
    strip_include_prefix = select({
        "@the8thwall//bzl/conditions:v1-linux": "x86_64-niantic_v1.0-linux-gnu/x86_64-niantic_v1.0-linux-gnu/include",
        "//conditions:default": "",
    }),
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:v1-linux": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)

filegroup(
    name = "hermetic_ldd",
    srcs = glob(["**/*-linux-gnu/sysroot/usr/bin/ldd"]),
)

filegroup(
    name = "hermetic_objdump",
    srcs = glob(["**/*-linux-gnu/bin/objdump"]),
)

filegroup(
    name = "hermetic_libgcc_s",
    srcs = glob(["**/*-linux-gnu/sysroot/lib/libgcc_s.so.1"]),
)

filegroup(
    name = "hermetic_libstdc++",
    srcs = glob(["**/*-linux-gnu/sysroot/lib/libstdc++.so*"]),
)
