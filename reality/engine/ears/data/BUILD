load("//bzl/utils:embedded.bzl", "embedded_data")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "earmodel",
    srcs = [
        "ear_model_v2.tflite",
    ],
)

embedded_data(
    name = "embedded-ear-model",
    srcs = [
        "ear_model_v2.tflite",
    ],
)
