load("//bzl/utils:embedded.bzl", "embedded_data")

licenses(["permissive"])

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "depthnet",
    srcs = [
        "depthnet.tflite",
    ],
)

embedded_data(
    name = "embedded-depthnet",
    srcs = [
        "depthnet.tflite",
    ],
)
