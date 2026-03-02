load("//bzl/js:js.bzl", "js_test")

cc_test(
    name = "proto-cc-test",
    size = "small",
    srcs = [
        "proto-cc-test.cc",
    ],
    data = [
        "//bzl/examples/proto/data:hello",
    ],
    deps = [
        "//bzl/examples/proto/api:hello-cc",
        "@com_google_googletest//:gtest_main",
    ],
)

js_test(
    name = "proto-ts-test",
    size = "small",
    data = [
        "//bzl/examples/proto/data:hello",
    ],
    main = [
        "proto-ts-test.ts",
    ],
    npm_rule = "@npm-bzl-examples//:npm-bzl-examples",
    deps = [
        "//bzl/examples/proto/api:hello-ts",
        "//bzl/js:chai-js",
    ],
)
