licenses(["permissive"])  # BSD 3-clause

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "fast-score",
    srcs = [
        "fast-score.cc",
    ],
    hdrs = [
        "fast-score.h",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "fast",
    srcs = [
        "fast.cc",
    ],
    hdrs = [
        "fast.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":fast-score",
        ":feature2d",
        ":keypoint",
        "//c8/stats:scope-timer",
        "//third_party/cvlite/core",
        "//third_party/neon:sse-neon",
    ],
)

cc_library(
    name = "feature2d",
    srcs = [
        "feature2d.cc",
    ],
    hdrs = [
        "feature2d.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        "//c8/stats:scope-timer",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "keypoint",
    srcs = [
        "keypoint.cc",
    ],
    hdrs = [
        "keypoint.h",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "matchers",
    srcs = [
        "matchers.cc",
    ],
    hdrs = [
        "matchers.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        "//third_party/cvlite/core",
        "//third_party/cvlite/flann:miniflann",
    ],
)
