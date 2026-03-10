licenses(["notice"])  # MIT License

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cnpy",
    srcs = [
        "cnpy.cpp",
    ],
    hdrs = [
        "cnpy.h",
    ],
    include_prefix = "cnpy",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "@zlib",
    ],
)

cc_test(
    name = "example1",
    srcs = [
        "example1.cpp",
    ],
    deps = [
        ":cnpy",
    ],
)
