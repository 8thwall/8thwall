load("@rules_python//python:defs.bzl", "py_library")
load("@pip-deps//:requirements.bzl", "requirement")

py_library(
    name = "python-gitlab",
    srcs = glob(["gitlab/**/*.py"]),
    imports = ["."],
    visibility = ["//visibility:public"],
    deps = [
        requirement("requests"),
        requirement("requests-toolbelt"),
    ],
)

