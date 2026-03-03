load("@bazel_skylib//lib:selects.bzl", "selects")
load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load(":node-opengl-polyfill-platform.bzl", "node_opengl_polyfill")
load("//bzl/js:js.bzl", "js_binary")

# When building a native node-addon to polyfill opengl in node, use this platform.
label_flag(
    name = "node-opengl-polyfill-platform",
    build_setting_default = "@local_execution_config_platform//:platform",
)

js_binary(
    name = "dom-polyfill",
    commonjs = True,
    export_library = 1,
    main = [
        "dom-polyfill.ts",
    ],
    npm_rule = "@npm-rendering//:npm-rendering",
    tags = ["manual"],
    target = "node",
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:wasm": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = [
        "//c8/dom",
    ],
)

node_opengl_polyfill(
    name = "dom-polyfill-for-node-polyfill-platform",
    actual = ":dom-polyfill",
    tags = ["manual"],
    visibility = ["//visibility:private"],
)

bool_flag(
    name = "angle",
    build_setting_default = False,
    visibility = [
        "//visibility:public",
    ],
)

config_setting(
    name = "with-angle",
    flag_values = {
        ":angle": "True",
    },
    visibility = [
        "//visibility:public",
    ],
)

config_setting(
    name = "without-angle",
    flag_values = {
        ":angle": "False",
    },
    visibility = [
        "//visibility:public",
    ],
)

config_setting(
    name = "android-without-angle",
    constraint_values = [
        "@platforms//os:android",
    ],
    flag_values = {
        ":angle": "False",
    },
)

cc_library(
    name = "gl-headers-with-angle",
    defines = [
        "C8_USE_ANGLE=1",
    ],
    tags = ["manual"],
    target_compatible_with = select({
        "@the8thwall//bzl/conditions:wasm": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }),
    deps = [
        "@angle//:EGL",
        "@angle//:GLESv2",
    ],
)

cc_library(
    name = "gl-headers-without-angle",
    defines = select({
        "@the8thwall//bzl/conditions:ios": [
            "GLES_SILENCE_DEPRECATION",
        ],
        "//bzl/conditions:osx": [
            "GL_SILENCE_DEPRECATION",
        ],
        "//conditions:default": [],
    }),
    linkopts = select({
        "@the8thwall//bzl/conditions:android": [
            "-lGLESv3",
        ],
        "@the8thwall//bzl/crosstool:amazonlinux": [
            "-lGL",
        ],
        "@the8thwall//bzl/ios:armv7": [
            "-framework OpenGLES",
        ],
        "@the8thwall//bzl/ios:arm64": [
            "-framework OpenGLES",
        ],
        "//bzl/conditions:osx": [
            "-framework OpenGL",
        ],
        "//conditions:default": [],
    }),
    deps = select({
        "//bzl/conditions:v1-linux": ["@v1-linux//:opengl"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "gl-headers",
    hdrs = [
        "egl.h",
        "eglext.h",
        "eglplatform.h",
        "gl.h",
        "gl-constants.h",
        "gl-version.h",
        "glext.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = select({
        ":with-angle": ["gl-headers-with-angle"],
        ":without-angle": ["gl-headers-without-angle"],
    }),
)

cc_library(
    name = "offscreen-gl-context-impl",
    hdrs = [
        "offscreen-gl-context-impl.h",
    ],
    visibility = [
        "//visibility:private",
    ],
)

deps_by_egl_support = selects.with_or({
    (
        ":with-angle",
        "@the8thwall//bzl/conditions:android",
        "@the8thwall//bzl/conditions:wasm",
        "@the8thwall//bzl/crosstool:amazonlinux",
        "@the8thwall//bzl/conditions:v1-linux",
    ): [
        ":offscreen-gl-context-egl-impl",
    ],
    "//conditions:default": [],
})

deps_by_cgl_support = selects.with_or({
    ("@the8thwall//bzl/conditions:osx",): [
        ":offscreen-gl-context-cgl-impl",
    ],
    "//conditions:default": [],
})

cc_library(
    name = "offscreen-gl-context",
    srcs = [
        "offscreen-gl-context.cc",
    ],
    hdrs = [
        "offscreen-gl-context.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
               "//c8:exceptions",
               ":gl-headers",
               ":offscreen-gl-context-impl",
           ] + deps_by_egl_support +
           deps_by_cgl_support,
)

cc_library(
    name = "client-gl",
    srcs = [
        "client-gl.cc",
    ],
    hdrs = [
        "client-gl.h",
    ],
    linkopts = select({
        ":with-angle": [],
        ":android-without-angle": [
            "-lEGL",
        ],
        "@the8thwall//bzl/crosstool:amazonlinux": [
            "-lEGL",
            "-lX11",
        ],
        "@the8thwall//bzl/conditions:v1-linux": [
            "-lEGL",
            "-lX11",
        ],
        "@the8thwall//bzl/conditions:wasm": [
            "-lEGL",
        ],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "offscreen-gl-context-cgl-impl",
    srcs = [
        "offscreen-gl-context-cgl-impl.cc",
    ],
    hdrs = [
        "offscreen-gl-context-cgl-impl.h",
    ],
    linkopts = select({
        "//bzl/conditions:osx": [
            "-framework OpenGL",
        ],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:private",
    ],
    deps = [
        ":gl-headers",
        ":offscreen-gl-context-impl",
        "//c8:exceptions",
        "//c8:string",
    ],
)

cc_library(
    name = "offscreen-gl-context-egl-impl",
    srcs = [
        "offscreen-gl-context-egl-impl.cc",
    ],
    hdrs = [
        "offscreen-gl-context-egl-impl.h",
    ],
    data = select({
        "@the8thwall//bzl/conditions:wasm": [
            ":dom-polyfill-for-node-polyfill-platform",
        ],
        "//conditions:default": [],
    }),
    linkopts = select({
        "@the8thwall//bzl/conditions:android": [
            "-lEGL",
        ],
        "@the8thwall//bzl/conditions:wasm": [
            "-lEGL",
        ],
        "//bzl/conditions:osx": [
            "-framework CoreFoundation",
            "-framework OpenGL",
        ],
        "@the8thwall//bzl/conditions:v1-linux": [
            "-lEGL",
            "-lX11",
        ],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:private",
    ],
    deps = [
        ":gl-headers",
        ":offscreen-gl-context-impl",
        "//c8:c8-log",
        "//c8:exceptions",
        "//c8:string",
    ],
)

cc_library(
    name = "gl-program",
    srcs = [
        "gl-program.cc",
    ],
    hdrs = [
        "gl-program.h",
    ],
    linkopts = select({
        "@the8thwall//bzl/conditions:android": [
            "-llog",
        ],
        "//conditions:default": [],
    }),
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        ":gl-version",
        "//c8:c8-log",
        "//c8:map",
        "//c8:string",
        "//c8:vector",
        "//c8/pixels/render:uniform",
        "//c8/string:format",
    ],
)

# START_AUTOGENERATED_RULES
# Do not edit these rules manually, use inliner.
# file: client-gl-test.cc
cc_test(
    name = "client-gl-test",
    size = "small",
    srcs = [
        "client-gl-test.cc",
    ],
    deps = [
        ":client-gl",
        ":gl-headers",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0xc0eed5e3)

# file: gl-buffer-object-test.cc
cc_test(
    name = "gl-buffer-object-test",
    size = "small",
    srcs = [
        "gl-buffer-object-test.cc",
    ],
    deps = [
        ":client-gl",
        ":gl-buffer-object",
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:scope-exit",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x1443a9d1)

# file: gl-buffer-object.cc
cc_library(
    name = "gl-buffer-object",
    srcs = [
        "gl-buffer-object.cc",
    ],
    hdrs = [
        "gl-buffer-object.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0x015bf574)

# cc_end(0x5200857f)

# file: gl-error.cc
cc_library(
    name = "gl-error",
    srcs = [
        "gl-error.cc",
    ],
    hdrs = [
        "gl-error.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:string",
    ],
)
# cc_end(0x6b96aac6)

# file: gl-extensions.cc
cc_library(
    name = "gl-extensions",
    srcs = [
        "gl-extensions.cc",
    ],
    hdrs = [
        "gl-extensions.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0x13746573)

# file: gl-framebuffer-test.cc
cc_test(
    name = "gl-framebuffer-test",
    size = "small",
    srcs = [
        "gl-framebuffer-test.cc",
    ],
    deps = [
        ":client-gl",
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":gl-quad",
        ":gl-texture",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:scope-exit",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x44cb9b0c)

# file: gl-framebuffer.cc
cc_library(
    name = "gl-framebuffer",
    srcs = [
        "gl-framebuffer.cc",
    ],
    hdrs = [
        "gl-framebuffer.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-error",
        ":gl-headers",
        ":gl-renderbuffer",
        ":gl-texture",
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0x72a5ad4a)

# file: gl-pixel-buffer.cc
cc_library(
    name = "gl-pixel-buffer",
    srcs = [
        "gl-pixel-buffer.cc",
    ],
    hdrs = [
        "gl-pixel-buffer.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-buffer-object",
        ":gl-headers",
        "//bzl/inliner:rules2",
        "//c8/pixels",
    ],
)
# cc_end(0x76154f10)

# file: gl-program-test.cc
cc_test(
    name = "gl-program-test",
    size = "small",
    srcs = [
        "gl-program-test.cc",
    ],
    deps = [
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":gl-vertex-array",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:hvector",
        "//c8:scope-exit",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0xe56bae42)

# file: gl-quad.cc
cc_library(
    name = "gl-quad",
    srcs = [
        "gl-quad.cc",
    ],
    hdrs = [
        "gl-quad.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        ":gl-vertex-array",
        "//bzl/inliner:rules2",
        "//c8:c8-log",
    ],
)
# cc_end(0xcd59a18e)

# file: gl-renderbuffer.cc
cc_library(
    name = "gl-renderbuffer",
    srcs = [
        "gl-renderbuffer.cc",
    ],
    hdrs = [
        "gl-renderbuffer.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0xad205aeb)

# file: gl-state-benchmark-test.cc
cc_test(
    name = "gl-state-benchmark-test",
    size = "small",
    srcs = [
        "gl-state-benchmark-test.cc",
    ],
    tags = [
        "manual",
    ],
    deps = [
        ":gl-headers",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "@com_google_benchmark//:benchmark",
    ],
)
# cc_end(0x03d0a68e)

# file: gl-texture-test.cc
cc_test(
    name = "gl-texture-test",
    size = "small",
    srcs = [
        "gl-texture-test.cc",
    ],
    deps = [
        ":client-gl",
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":gl-quad",
        ":gl-texture",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:scope-exit",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
        "//c8/pixels:test-image",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x7e333869)

# file: gl-texture.cc
cc_library(
    name = "gl-texture",
    srcs = [
        "gl-texture.cc",
    ],
    hdrs = [
        "gl-texture.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:string",
        "//c8/pixels/render:texture",
        "//c8/string:format",
    ],
)
# cc_end(0xe9902e7a)

# file: gl-version.cc
cc_library(
    name = "gl-version",
    srcs = [
        "gl-version.cc",
    ],
    hdrs = [
        "gl-version.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gl-headers",
        "//bzl/inliner:rules2",
    ],
)
# cc_end(0x45e3181f)

# file: gl-vertex-array-test.cc
cc_test(
    name = "gl-vertex-array-test",
    size = "small",
    srcs = [
        "gl-vertex-array-test.cc",
    ],
    deps = [
        ":client-gl",
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":gl-vertex-array",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:scope-exit",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x904d026e)

# file: gl-vertex-array.cc
cc_library(
    name = "gl-vertex-array",
    srcs = [
        "gl-vertex-array.cc",
    ],
    hdrs = [
        "gl-vertex-array.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":client-gl",
        ":gl-buffer-object",
        ":gl-headers",
        "//bzl/inliner:rules2",
        "//c8:exceptions",
        "//c8:map",
    ],
)
# cc_end(0xf9a383ab)

# file: offscreen-gl-context-test.cc
cc_test(
    name = "offscreen-gl-context-test",
    size = "small",
    srcs = [
        "offscreen-gl-context-test.cc",
    ],
    deps = [
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-program",
        ":gl-quad",
        ":gl-texture",
        ":offscreen-gl-context",
        "//bzl/inliner:rules2",
        "//c8:scope-exit",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x462d39af)

# file: texture-transforms-test.cc
cc_test(
    name = "texture-transforms-test",
    size = "small",
    srcs = [
        "texture-transforms-test.cc",
    ],
    deps = [
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-pixel-buffer",
        ":gl-texture",
        ":offscreen-gl-context",
        ":texture-transforms",
        "//bzl/inliner:rules2",
        "//c8/pixels:pixel-buffer",
        "@com_google_googletest//:gtest_main",
    ],
)
# cc_end(0x859614ea)

# file: texture-transforms.cc
cc_library(
    name = "texture-transforms",
    srcs = [
        "texture-transforms.cc",
    ],
    hdrs = [
        "texture-transforms.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        ":client-gl",
        ":gl-buffer-object",
        ":gl-extensions",
        ":gl-framebuffer",
        ":gl-headers",
        ":gl-pixel-buffer",
        ":gl-program",
        ":gl-quad",
        ":gl-texture",
        ":gl-vertex-array",
        "//bzl/inliner:rules2",
        "//c8:hmatrix",
        "//c8/geometry:parameterized-geometry",
        "//c8/pixels",
        "//c8/pixels:pixel-buffer",
    ],
)
# cc_end(0x5e4fe8cb)
# END_AUTOGENERATED_RULES
