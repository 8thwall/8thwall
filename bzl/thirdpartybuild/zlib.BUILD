licenses(["permissive"])

load("@the8thwall//bzl/windows:select.bzl", "onWindows")

cc_library(
    name = "zlib",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-w",
        "-Dverbose=-1",
        "-Wno-deprecated-non-prototype",
    ] + onWindows(
        [
            "-Wshorten-64-to-32",
            "-Wattributes",
            "-Wstrict-prototypes",
            "-Wmissing-prototypes",
            "-Wmissing-declarations",
            "-Wshift-negative-value",
            "-D_CRT_SECURE_NO_DEPRECATE",
            "-D_CRT_NONSTDC_NO_DEPRECATE",
        ],
        ["-DZ_HAVE_UNISTD_H=1"],
    ),
    includes = ["."],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "minizip",
    srcs = [
        "contrib/minizip/crypt.h",
        "contrib/minizip/ioapi.c",
        "contrib/minizip/ioapi.h",
        "contrib/minizip/unzip.c",
        "contrib/minizip/zip.c",
    ],
    hdrs = [
        "contrib/minizip/unzip.h",
        "contrib/minizip/zip.h",
    ],
    defines = [
        "MINIZIP_FOPEN_NO_64",
        "IOAPI_NO_64",
    ],
    includes = ["include"],
    visibility = ["//visibility:public"],
)
