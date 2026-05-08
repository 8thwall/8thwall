licenses(["permissive"])  # BSD

# Facebook Zstd (https://github.com/facebook/zstd), v1.5.6 layout.
cc_library(
    name = "zstd",
    srcs = glob(
        [
            "lib/common/*.c",
            "lib/compress/*.c",
            "lib/decompress/*.c",
            "lib/dictBuilder/*.c",
            "lib/deprecated/*.c",
            "lib/legacy/*.c",
        ],
    ),
    hdrs = glob([
        "lib/*.h",
        "lib/common/*.h",
        "lib/compress/*.h",
        "lib/decompress/*.h",
        "lib/dictBuilder/*.h",
        "lib/deprecated/*.h",
        "lib/legacy/*.h",
    ]),
    copts = [
        "-w",
    ] + select({
        "@the8thwall//bzl/conditions:arm64": [
            "-DZSTD_DISABLE_ASM",
        ],
        "//conditions:default": [],
    }),
    includes = ["lib"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
