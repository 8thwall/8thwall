licenses(["permissive"])  # BSD 3-clause

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "distortion-model",
    hdrs = [
        "distortion-model.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "drawing",
    srcs = [
        "drawing.cc",
        "hershey_fonts.cc",
    ],
    hdrs = [
        "drawing.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "filter",
    srcs = [
        "filter.cc",
    ],
    hdrs = [
        "filter.h",
        "filterengine.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":hal-replacement",
        ":types-c",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "hal-replacement",
    hdrs = [
        "hal-replacement.h",
    ],
    deps = [
        ":interface",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "imgwarp",
    srcs = [
        "imgwarp.cc",
    ],
    hdrs = [
        "imgwarp.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":hal-replacement",
        ":types-c",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "interface",
    hdrs = [
        "interface.h",
    ],
)

cc_library(
    name = "precomp",
    srcs = [
        "tables.cc",
    ],
    hdrs = [
        "precomp.h",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "smooth",
    srcs = [
        "smooth.cc",
    ],
    hdrs = [
        "smooth.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":filter",
        ":hal-replacement",
        ":precomp",
        ":types-c",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "thresh",
    srcs = [
        "thresh.cc",
    ],
    hdrs = [
        "thresh.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":hal-replacement",
        ":smooth",
        ":types-c",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "types-c",
    hdrs = [
        "types_c.h",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "undistort",
    srcs = [
        "undistort.cc",
    ],
    hdrs = [
        "undistort.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":distortion-model",
        ":imgwarp",
        "//third_party/cvlite/core",
    ],
)
