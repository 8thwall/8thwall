load("@the8thwall//bzl/capnproto:capnproto.bzl", "cc_capnp_library")

licenses(["permissive"])  # MIT

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "capnp-cs-annotations",
    srcs = [
        "compiler/capnp/cs.capnp",
    ],
)

cc_capnp_library(
    name = "schema-capnp-cs",
    srcs = ["compiler/capnp/cs.capnp"],
)

cc_binary(
    name = "capnpc-cs",
    srcs = [
        "compiler/capnpc-cs.c++",
    ],
    copts = [
        "-Wno-unused-function",
        "-Wno-unused-const-variable",
        "-Wno-unused-variable",
        "-Wno-enum-compare-switch",
    ],
    deps = [
        ":schema-capnp-cs",
        "@capnproto//:capnp-lib",
        "@capnproto//:kj",
    ],
)
