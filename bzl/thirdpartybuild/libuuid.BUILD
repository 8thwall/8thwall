cc_library(
    name = "libuuid",
    srcs = [
        "all-io.h",
        "c.h",
        "clear.c",
        "compare.c",
        "copy.c",
        "gen_uuid.c",
        "isnull.c",
        "pack.c",
        "parse.c",
        "randutils.c",
        "randutils.h",
        "unpack.c",
        "unparse.c",
        "uuidP.h",
        "uuid_time.c",
        "uuidd.h",
    ],
    hdrs = [
        "uuid.h",
    ],
    copts = [
        "-DHAVE_USLEEP=1",
    ] + select({
        "@the8thwall//bzl/conditions:android": [
            "-DHAVE_SYS_FILE_H=1",
        ],
        "@the8thwall//bzl/conditions:v1-linux": [
            "-DHAVE_SYS_FILE_H=1",
        ],
        "@the8thwall//bzl/conditions:v2-linux": [
            "-DHAVE_SYS_FILE_H=1",
        ],
        "//conditions:default": [],
    }),
    include_prefix = "uuid",
    visibility = ["//visibility:public"],
    deps = [],
)
