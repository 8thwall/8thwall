licenses(["permissive"])  # BSD 2-clause

cc_library(
    name = "openh264",
    hdrs = [
        ":public-headers",
    ],
    copts = [
        "-Iexternal/openh264/codec/processing/interface",
        "-Iexternal/openh264/codec/decoder/core/inc",
        "-Iexternal/openh264/codec/encoder/core/inc",
        "-Iexternal/openh264/codec/common/inc",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_PTHREADS",
        ],
        "//conditions:default": [],
    }),
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":decoder",
        ":encoder",
    ],
)

genrule(
    name = "public-headers",
    srcs = [
        "codec/api/svc/codec_api.h",
        "codec/api/svc/codec_app_def.h",
        "codec/api/svc/codec_def.h",
        "codec/api/svc/codec_ver.h",
    ],
    outs = [
        "include/openh264/codec_api.h",
        "include/openh264/codec_app_def.h",
        "include/openh264/codec_def.h",
        "include/openh264/codec_ver.h",
    ],
    cmd = "cp $(SRCS) $(@D)/include/openh264/",
    visibility = ["//visibility:private"],
)

cc_library(
    name = "common",
    srcs = glob([
        "codec/common/src/*.cpp",
    ]),
    hdrs = glob([
        "codec/common/inc/*.h",
        "codec/api/svc/*.h",
    ]),
    copts = [
        "-Iexternal/openh264/codec/api/svc",
        "-Iexternal/openh264/codec/common/inc",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_PTHREADS",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
)

cc_library(
    name = "processing",
    srcs = glob([
        "codec/processing/src/adaptivequantization/*.cpp",
        "codec/processing/src/adaptivequantization/*.h",
        "codec/processing/src/backgrounddetection/*.cpp",
        "codec/processing/src/backgrounddetection/*.h",
        "codec/processing/src/common/*.cpp",
        "codec/processing/src/common/*.h",
        "codec/processing/src/complexityanalysis/*.cpp",
        "codec/processing/src/complexityanalysis/*.h",
        "codec/processing/src/denoise/*.cpp",
        "codec/processing/src/denoise/*.h",
        "codec/processing/src/downsample/*.cpp",
        "codec/processing/src/downsample/*.h",
        "codec/processing/src/imagerotate/*.cpp",
        "codec/processing/src/imagerotate/*.h",
        "codec/processing/src/scrolldetection/*.cpp",
        "codec/processing/src/scrolldetection/*.h",
        "codec/processing/src/scenechangedetection/*.cpp",
        "codec/processing/src/scenechangedetection/*.h",
        "codec/processing/src/vaacalc/*.cpp",
        "codec/processing/src/vaacalc/*.h",
    ]),
    hdrs = glob([
        "codec/processing/interface/*.h",
    ]),
    copts = [
        "-Iexternal/openh264/codec/processing/interface",
        "-Iexternal/openh264/codec/processing/src/common",
        "-Iexternal/openh264/codec/api/svc",
        "-Iexternal/openh264/codec/common/inc",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_PTHREADS",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = [
        ":common",
    ],
)

cc_library(
    name = "encoder",
    srcs = glob([
        "codec/encoder/core/src/*.cpp",
        "codec/encoder/core/inc/*.h",
        "codec/encoder/plus/src/welsEncoderExt.cpp",
        "codec/encoder/plus/inc/*.h",
    ]),
    copts = [
        "-Iexternal/openh264/codec/api/svc",
        "-Iexternal/openh264/codec/encoder/core/inc",
        "-Iexternal/openh264/codec/encoder/plus/inc",
        "-Iexternal/openh264/codec/common/inc",
        "-Iexternal/openh264/codec/processing/interface",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_PTHREADS",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = [
        ":common",
        ":processing",
    ],
)

cc_library(
    name = "decoder",
    srcs = glob([
        "codec/decoder/core/src/*.cpp",
        "codec/decoder/core/inc/*.h",
        "codec/decoder/plus/src/welsDecoderExt.cpp",
        "codec/decoder/plus/inc/*.h",
    ]),
    copts = [
        "-Iexternal/openh264/codec/api/svc",
        "-Iexternal/openh264/codec/decoder/core/inc",
        "-Iexternal/openh264/codec/decoder/plus/inc",
        "-Iexternal/openh264/codec/common/inc",
        "-Iexternal/openh264/codec/processing/interface",
    ] + select({
        "@the8thwall//bzl/conditions:wasm": [
            "-DNO_PTHREADS",
        ],
        "//conditions:default": [],
    }),
    visibility = ["//visibility:private"],
    deps = [
        ":common",
        ":processing",
    ],
)
