licenses(["permissive"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "giflib",
    srcs = [
        "dgif_lib.c",
        "egif_lib.c",
        "gif_err.c",
        "gif_font.c",
        "gif_hash.c",
        "gif_hash.h",
        "gif_lib_private.h",
        "gifalloc.c",
        "openbsd-reallocarray.c",
    ],
    hdrs = ["gif_lib.h"],
    includes = ["."],
    visibility = ["//visibility:public"],
)
