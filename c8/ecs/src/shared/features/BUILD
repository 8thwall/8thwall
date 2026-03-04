load("//bzl/js:js.bzl", "js_library")

package(default_visibility = [
    "//c8/ecs:__subpackages__",
    "//reality/cloud:shared",
])

filegroup(
    name = "features-files",
    srcs = glob(
        ["*.ts"],
        exclude = ["edition.ts"],
    ),
)

js_library(
    name = "features",
    srcs = [":features-files"],
    visibility = ["//visibility:public"],
)

js_library(
    name = "edition",
    srcs = [
        "edition.ts",
    ],
)
