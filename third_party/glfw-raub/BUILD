load("@the8thwall//bzl/js:js.bzl", "js_binary", "js_library")

exports_files(["glfw-raub.patch"])

js_library(
    name = "glfw-raub",
    srcs = [
        "constants.js",
        "document.js",
        "glfw-raub.js",
        "window.js",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@glfw-raub//:glfw-raub",
    ],
)
