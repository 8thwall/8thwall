load("//bzl/js:js.bzl", "js_library", "js_test")

package(default_visibility = ["//visibility:public"])

js_library(
    name = "args",
    srcs = [
        "args.ts",
    ],
)

js_test(
    name = "args-test",
    main = [
        "args-test.ts",
    ],
    deps = [
        ":args",
        "//bzl/js:chai-js",
    ],
)

js_library(
    name = "proc",
    srcs = [
        "proc.ts",
    ],
)

js_library(
    name = "s3",
    srcs = [
        "s3.ts",
    ],
    deps = [
        ":proc",
    ],
)

js_library(
    name = "cert",
    srcs = [
        "cert.ts",
    ],
)

js_library(
    name = "ip",
    srcs = [
        "ip.ts",
    ],
)

js_library(
    name = "hugging-face",
    srcs = [
        "hugging-face.ts",
    ],
    deps = [
        ":proc",
    ],
)

js_test(
    name = "proc-test",
    size = "small",
    main = [
        "proc-test.ts",
    ],
    deps = [
        "//bzl/js:chai-js",
        "//c8/cli:proc",
    ],
)
