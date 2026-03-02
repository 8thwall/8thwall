cc_library(
    name = "hello-world-x64",
    srcs = [
        "hello-world-x64.asm",
    ],
    target_compatible_with = [
        "@platforms//os:windows",
    ],
)

cc_test(
    name = "hello-world-x64-test",
    size = "small",
    srcs = [
        "hello-world-x64-test.cc",
    ],
    target_compatible_with = [
        "@platforms//os:windows",
    ],
    deps = [
        ":hello-world-x64",
        "//bzl/inliner:rules2",
        "@com_google_googletest//:gtest_main",
    ],
)
