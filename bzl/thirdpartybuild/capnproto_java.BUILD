load("@the8thwall//bzl/capnproto:capnproto.bzl", "cc_capnp_library")

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "capnp-java-annotations",
    srcs = [
        "compiler/src/main/schema/capnp/java.capnp",
    ],
)

cc_capnp_library(
    name = "schema-capnp-java",
    srcs = ["compiler/src/main/schema/capnp/java.capnp"],
)

cc_binary(
    name = "capnpc-java",
    srcs = [
        "compiler/src/main/cpp/capnpc-java.c++",
    ],
    copts = [
        "-Wno-unused-function",
        "-Wno-unused-const-variable",
        "-Wno-unused-variable",
        "-Wno-enum-compare-switch",
    ],
    deps = [
        ":schema-capnp-java",
        "@capnproto//:capnp-lib",
        "@capnproto//:kj",
    ],
)

java_library(
    name = "capnp-runtime-java",
    srcs = glob(["runtime/src/main/java/org/capnproto/*.java"]),
)
