load("//bzl/capnproto:capnproto.bzl", "capnp_library", "cc_capnp_library")

capnp_library(
    name = "log-request",
    srcs = [
        "log-request.capnp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//c8/stats/api:detail",
        "//c8/stats/api:summary",
        "//reality/engine/api:reality",
        "//reality/engine/api/device:info",
    ],
)

capnp_library(
    name = "remote-request",
    srcs = [
        "remote-request.capnp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":log-request",
        "//reality/engine/api:reality",
        "//reality/engine/api/base:image-types",
    ],
)

# Currently java capnproto doesn't support interfaces (RPCs); here we only provide a cc library for
# the service itself.
cc_capnp_library(
    # Mimic the naming of normal generated cc_capnp targets so that we can add java support easily
    # later.
    name = "log-service.capnp-cc",
    srcs = [
        "log-service-interface.capnp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        # TODO(nb): remove .capnp-cc from this dep when we support java interfaces too.
        ":log-request.capnp-cc",
    ],
)

cc_capnp_library(
    # Mimic the naming of normal generated cc_capnp targets so that we can add java support easily
    # later.
    name = "remote-service.capnp-cc",
    srcs = [
        "remote-service-interface.capnp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        # TODO(nb): remove .capnp-cc from this dep when we support java interfaces too.
        ":log-request.capnp-cc",
        ":remote-request.capnp-cc",
    ],
)
