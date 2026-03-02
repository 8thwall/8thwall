package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "load-spz",
    srcs = [
        "load-spz.cc",
    ],
    hdrs = [
        "load-spz.h",
    ],
    deps = [
        ":splat-types",
        "@zlib",
    ],
)

cc_test(
    name = "load-spz-test",
    srcs = [
        "load-spz-test.cc",
    ],
    deps = [
        ":load-spz",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "splat-types",
    srcs = [
        "splat-types.cc",
    ],
    hdrs = [
        "splat-types.h",
    ],
    deps = [
        "splat-c-types",
    ],
)

cc_test(
    name = "splat-types-test",
    srcs = [
        "splat-types-test.cc",
    ],
    deps = [
        ":splat-types",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "splat-c-types",
    srcs = [
        "splat-c-types.cc",
    ],
    hdrs = [
        "splat-c-types.h",
    ],
    deps = [
    ],
)
