load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "implot",
    srcs = [
        "implot.cpp",
        "implot_internal.h",
        "implot_items.cpp",
    ],
    hdrs = [
        "implot.h",
    ],
    visibility = [
        "//visibility:public",
    ],
    deps = [
        "@imgui",
    ],
)
