licenses(["permissive"])  # BSD 3-clause

cc_library(
    name = "libyuv",
    srcs =
        glob(["source/*.cc"]),
    hdrs = glob([
        "include/libyuv.h",
        "include/libyuv/*.h",
    ]),
    copts = [
        "-Iexternal/libvpx/third_party/libyuv/include",
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
