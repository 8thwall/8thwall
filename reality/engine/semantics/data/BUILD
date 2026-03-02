load("//bzl/utils:embedded.bzl", "embedded_data")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "semanticsnetportrait",
    srcs = [
        # https://console.cloud.google.com/storage/browser/res-8thwall-xfn/models/semantics_model_selection_march23/mobilenetv3_separable/quarter/fp16
        "semantics_mobilenetv3_separable_quarter_fp16.tflite",
    ],
)

embedded_data(
    name = "embedded-semanticsnet-portrait",
    srcs = [
        # https://console.cloud.google.com/storage/browser/res-8thwall-xfn/models/semantics_model_selection_march23/mobilenetv3_separable/quarter/fp16
        "semantics_mobilenetv3_separable_quarter_fp16.tflite",
    ],
)
