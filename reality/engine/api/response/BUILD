load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(default_visibility = ["//visibility:public"])

capnp_library(
    name = "features",
    srcs = [
        "features.capnp",
    ],
    deps = [
        ":status",
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:feature-types",
        "//reality/engine/api/base:geo-types",
    ],
)

capnp_library(
    name = "pose",
    srcs = [
        "pose.capnp",
    ],
    deps = [
        ":status",
        "//reality/engine/api/base:geo-types",
    ],
)

capnp_library(
    name = "sensor-test",
    srcs = [
        "sensor-test.capnp",
    ],
    deps = [
        ":status",
        "//reality/engine/api/base:geo-types",
    ],
)

capnp_library(
    name = "status",
    srcs = [
        "status.capnp",
    ],
    deps = [
    ],
)
