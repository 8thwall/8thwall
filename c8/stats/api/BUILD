load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(default_visibility = ["//visibility:public"])

capnp_library(
    name = "detail",
    srcs = [
        "detail.capnp",
    ],
    deps = [
    ],
)

capnp_library(
    name = "histogram-types",
    srcs = ["histogram-types.capnp"],
)

capnp_library(
    name = "summary",
    srcs = [
        "summary.capnp",
    ],
    deps = [
        ":histogram-types",
    ],
)
