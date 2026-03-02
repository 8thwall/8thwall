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
    copts = ["-DHAVE_POSIX_REGEX"],  # HAVE_STD_REGEX didn't work.
    includes = ["include"],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
