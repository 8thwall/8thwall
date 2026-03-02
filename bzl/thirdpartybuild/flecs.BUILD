licenses(["permissive"])  # MIT

cc_library(
    name = "flecs",
    srcs = glob(
        [
            "src/**",
        ],
    ),
    hdrs = glob([
        "include/**/*.h",
        "include/**/*.hpp",
        "include/**/*.inl",
    ]),
    includes = [
        ".",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
