load("//bzl/js:js.bzl", "js_cli")

cc_library(
    name = "benchmark-dataset",
    testonly = 1,
    srcs = [
        "benchmark-dataset.cc",
    ],
    hdrs = [
        "benchmark-dataset.h",
    ],
    data = [
        "//reality/quality/datasets/lobbytour10:lobbytour10-480x640",
        "//reality/quality/datasets/simple10:simple10-240x320",
        "//reality/quality/datasets/simple10:simple10-480x640",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "//bzl/inliner:rules",
        "//c8:map",
        "//c8:string",
        "//c8:vector",
        "//c8/io:image-io",
        "//c8/pixels:pixel-buffer",
    ],
)

