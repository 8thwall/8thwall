load("@the8thwall//bzl/capnproto:capnproto.bzl", "cc_capnp_library")

package(default_visibility = ["//visibility:public"])

# TODO(dat): Make this project buildable so we don't have to install pip3 install pycapnp
#            on each machine. Currently, the project only provide the c++.capnp file
filegroup(
    name = "capnp-python-annotations",
    srcs = [
        "capnp/c++.capnp",
    ],
)
