load("//bzl/js:js.bzl", "js_library")

package(default_visibility = ["//reality/cloud:shared"])

js_library(
    name = "typed-attributes",
    srcs = [
        "typed-attributes.ts",
    ],
    visibility = [
        "//visibility:public",
    ],
)

js_library(
    name = "dynamodb",
    srcs = [
        "dynamodb.ts",
    ],
    visibility = ["//visibility:public"],
)

js_library(
    name = "s3",
    srcs = [
        "s3.ts",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//reality/cloud/xrhome/src/shared:registry",
    ],
)

js_library(
    name = "run-queue",
    srcs = [
        "run-queue.ts",
    ],
    visibility = ["//visibility:public"],
)
