load("//bzl/utils:embedded.bzl", "embedded_data")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "selfiemulticlass",
    srcs = [
        "selfie_multiclass_256x256.tflite",
    ],
)

embedded_data(
    name = "embedded-selfiemulticlass",
    srcs = [
        "selfie_multiclass_256x256.tflite",
    ],
)
