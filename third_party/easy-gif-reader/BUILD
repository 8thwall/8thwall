licenses(["permissive"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "easy-gif-reader",
    srcs = [
        "easy-gif-reader.cc",
    ],
    hdrs = [
        "easy-gif-reader.h",
    ],
    deps = [
        "//bzl/inliner:rules2",
        "//c8:c8-log",
        "//c8:exceptions",
        "//c8:string",
        "@giflib",
    ],
)
