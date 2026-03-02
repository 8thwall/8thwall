load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(default_visibility = ["//visibility:public"])

capnp_library(
    name = "app",
    srcs = [
        "app.capnp",
    ],
    deps = [
    ],
)

capnp_library(
    name = "flags",
    srcs = [
        "flags.capnp",
    ],
    deps = [
    ],
)

capnp_library(
    name = "hit-test",
    srcs = [
        "hit-test.capnp",
    ],
    deps = [
        ":sensor",
        "//reality/engine/api/base:geo-types",
    ],
)

capnp_library(
    name = "mask",
    srcs = [
        "mask.capnp",
    ],
    deps = [
    ],
)

capnp_library(
    name = "precomputed",
    srcs = [
        "precomputed.capnp",
    ],
    deps = [
    ],
)

capnp_library(
    name = "sensor",
    srcs = [
        "sensor.capnp",
    ],
    deps = [
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:geo-types",
        "//reality/engine/api/base:image-types",
    ],
)
