licenses(["permissive"])  # BSD

cc_library(
    name = "spdlog",
    srcs = glob([
        "src/*.cpp",
    ]),
    hdrs = glob([
        "include/spdlog/*.h",
        "include/spdlog/**/*.h",
    ]),
    defines = [
        "SPDLOG_COMPILED_LIB",
    ] + select({
        "@the8thwall//bzl/conditions:windows": [
            "SPDLOG_USE_STD_FORMAT",
        ],
        "//conditions:default": [],
    }),
    includes = ["include"],
    visibility = ["//visibility:public"],
    #deps = ["@com_github_fmtlib_fmt//:fmtlib"],
)
