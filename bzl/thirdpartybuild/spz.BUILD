licenses(["permissive"])  # MIT

cc_library(
    name = "spz",
    srcs = [
        "src/cc/load-spz.cc",
        "src/cc/splat-c-types.cc",
        "src/cc/splat-types.cc",
    ],
    hdrs = [
        "src/cc/load-spz.h",
        "src/cc/splat-c-types.h",
        "src/cc/splat-types.h",
    ],
    copts = ["-std=c++17"],
    include_prefix = "spz",
    strip_include_prefix = "src/cc",
    visibility = ["//visibility:public"],
    deps = [
        "@zlib",
        "@zstd",
    ],
)
