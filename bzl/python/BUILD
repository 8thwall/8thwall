load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")
load("@bazel_skylib//lib:selects.bzl", "selects")

genrule(
    name = "test_current_py_toolchain",
    srcs = [],
    outs = ["out.txt"],
    cmd = "$(PYTHON3) --version > $(location out.txt)",
    toolchains = ["@rules_python//python:current_py_toolchain"],
)

exports_files([
    "requirements.txt",
    "a4lidartag_requirements.txt",
])

# Hermetic python toolchains

py_runtime(
    name = "python3.9_runtime",
    files = ["@python-3.9//:files"],
    interpreter = "@python-3.9//:python3",
    python_version = "PY3",
    target_compatible_with = selects.with_or({
        ("//bzl/conditions:macos", "//bzl/conditions:linux", "//bzl/conditions:windows"): [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    visibility = ["//visibility:public"],
)

py_runtime_pair(
    name = "python3.9_runtime_pair",
    py2_runtime = None,
    py3_runtime = ":python3.9_runtime",
)

toolchain(
    name = "python3.9_toolchain",
    toolchain = ":python3.9_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
)
