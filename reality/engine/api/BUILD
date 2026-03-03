load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(default_visibility = ["//visibility:public"])

capnp_library(
    name = "face",
    srcs = [
        "face.capnp",
    ],
    deps = [
        "//reality/engine/api:mesh",
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:geo-types",
        "//reality/engine/api/base:image-types",
    ],
)

capnp_library(
    name = "hand",
    srcs = [
        "hand.capnp",
    ],
    deps = [
        "//reality/engine/api:mesh",
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:geo-types",
        "//reality/engine/api/base:image-types",
    ],
)

capnp_library(
    name = "mesh",
    srcs = [
        "mesh.capnp",
    ],
    deps = [
        "//reality/engine/api/base:geo-types",
    ],
)

capnp_library(
    name = "semantics",
    srcs = [
        "semantics.capnp",
    ],
    deps = [
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:geo-types",
        "//reality/engine/api/base:image-types",
    ],
)

capnp_library(
    name = "reality",
    srcs = [
        "reality.capnp",
    ],
    deps = [
        "//reality/engine/api/base:camera-intrinsics",
        "//reality/engine/api/base:debug",
        "//reality/engine/api/base:geo-types",
        "//reality/engine/api/base:id",
        "//reality/engine/api/base:image-types",
        "//reality/engine/api/base:lighting",
        "//reality/engine/api/device:info",
        "//reality/engine/api/request:app",
        "//reality/engine/api/request:flags",
        "//reality/engine/api/request:mask",
        "//reality/engine/api/request:sensor",
        "//reality/engine/api/response:features",
        "//reality/engine/api/response:pose",
        "//reality/engine/api/response:sensor-test",
        "//reality/engine/api/response:status",
    ],
)
