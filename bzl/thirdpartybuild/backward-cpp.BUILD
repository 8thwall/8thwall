licenses(["permissive"])  # MIT

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "backward-cpp",
    srcs = [
        "backward.cpp",
    ],
    hdrs = [
        "backward.hpp",
    ],
    copts = [
        "-DBACKWARD_SYSTEM_DARWIN=1",
    ],
    includes = ["."],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
