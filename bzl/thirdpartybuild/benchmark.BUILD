licenses(["notice"])

cc_library(
    name = "benchmark",
    srcs = glob([
        "src/*.h",
        "src/*.cc",
    ]),
    hdrs = [
        "include/benchmark/benchmark.h",
        "include/benchmark/export.h",
    ],
    defines = select({
        "@the8thwall//bzl/conditions:windows": [
            "BENCHMARK_STATIC_DEFINE",
            "HAVE_STD_REGEX",
        ],
        "//conditions:default": ["HAVE_POSIX_REGEX"],
    }),
    includes = ["include"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
