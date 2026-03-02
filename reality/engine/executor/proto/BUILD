load("//bzl/capnproto:capnproto.bzl", "capnp_library")

package(
    default_visibility = [
        "//reality/engine:__subpackages__",
    ],
)

capnp_library(
    name = "state",
    srcs = [
        "state.capnp",
    ],
    deps = [
        "//reality/engine/api:reality",
    ],
)
