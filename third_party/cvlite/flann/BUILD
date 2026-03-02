licenses(["permissive"])  # BSD 3-clause

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "all-indices",
    hdrs = [
        "all-indices.h",
    ],
    deps = [
        ":autotuned-index",
        ":composite-index",
        ":general",
        ":hierarchical-clustering-index",
        ":kdtree-index",
        ":kdtree-single-index",
        ":kmeans-index",
        ":linear-index",
        ":lsh-index",
        ":nn-index",
    ],
)

cc_library(
    name = "allocator",
    hdrs = [
        "allocator.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "any",
    hdrs = [
        "any.h",
    ],
    deps = [
        ":defines",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "autotuned-index",
    hdrs = [
        "autotuned-index.h",
    ],
    deps = [
        ":composite-index",
        ":general",
        ":ground-truth",
        ":index-testing",
        ":kdtree-index",
        ":kdtree-single-index",
        ":kmeans-index",
        ":linear-index",
        ":logger",
        ":nn-index",
        ":sampling",
    ],
)

cc_library(
    name = "composite-index",
    hdrs = [
        "composite-index.h",
    ],
    deps = [
        ":general",
        ":kdtree-index",
        ":kmeans-index",
        ":nn-index",
    ],
)

cc_library(
    name = "config",
    hdrs = [
        "config.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "defines",
    hdrs = [
        "defines.h",
    ],
    deps = [
        ":config",
    ],
)

cc_library(
    name = "dist",
    hdrs = [
        "dist.h",
    ],
    deps = [
        ":defines",
    ],
)

cc_library(
    name = "dummy",
    hdrs = [
        "dummy.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "dynamic-bitset",
    hdrs = [
        "dynamic-bitset.h",
    ],
    deps = [
        ":dist",
    ],
)

cc_library(
    name = "flann-base",
    hdrs = [
        "flann-base.h",
    ],
    deps = [
        ":all-indices",
        ":general",
        ":matrix",
        ":params",
        ":saving",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "flann",
    srcs = [
        "flann.cc",
    ],
    hdrs = [
        "flann.h",
    ],
    deps = [
        ":flann-base",
        ":miniflann",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "general",
    hdrs = [
        "general.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "ground-truth",
    hdrs = [
        "ground-truth.h",
    ],
    deps = [
        ":dist",
        ":matrix",
    ],
)

cc_library(
    name = "heap",
    hdrs = [
        "heap.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "hierarchical-clustering-index",
    hdrs = [
        "hierarchical-clustering-index.h",
    ],
    deps = [
        ":allocator",
        ":dist",
        ":general",
        ":heap",
        ":matrix",
        ":nn-index",
        ":random",
        ":result-set",
        ":saving",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "index-testing",
    hdrs = [
        "index-testing.h",
    ],
    deps = [
        ":logger",
        ":matrix",
        ":nn-index",
        ":result-set",
        ":timer",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "kdtree-index",
    hdrs = [
        "kdtree-index.h",
    ],
    deps = [
        ":allocator",
        ":dynamic-bitset",
        ":general",
        ":heap",
        ":matrix",
        ":nn-index",
        ":random",
        ":result-set",
        ":saving",
    ],
)

cc_library(
    name = "kdtree-single-index",
    hdrs = [
        "kdtree-single-index.h",
    ],
    deps = [
        ":allocator",
        ":general",
        ":heap",
        ":matrix",
        ":nn-index",
        ":random",
        ":result-set",
        ":saving",
    ],
)

cc_library(
    name = "kmeans-index",
    hdrs = [
        "kmeans-index.h",
    ],
    deps = [
        ":allocator",
        ":dist",
        ":general",
        ":heap",
        ":logger",
        ":matrix",
        ":nn-index",
        ":random",
        ":result-set",
        ":saving",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "linear-index",
    hdrs = [
        "linear-index.h",
    ],
    deps = [
        ":general",
        ":nn-index",
    ],
)

cc_library(
    name = "logger",
    hdrs = [
        "logger.h",
    ],
    deps = [
        ":defines",
    ],
)

cc_library(
    name = "lsh-index",
    hdrs = [
        "lsh-index.h",
    ],
    deps = [
        ":allocator",
        ":general",
        ":heap",
        ":lsh-table",
        ":matrix",
        ":nn-index",
        ":random",
        ":result-set",
        ":saving",
        "//c8:c8-log",
    ],
)

cc_library(
    name = "lsh-table",
    hdrs = [
        "lsh-table.h",
    ],
    deps = [
        ":dynamic-bitset",
        ":matrix",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "matrix",
    hdrs = [
        "matrix.h",
    ],
    deps = [
        ":defines",
        ":general",
    ],
)

cc_library(
    name = "miniflann",
    srcs = [
        "miniflann.cc",
    ],
    hdrs = [
        "miniflann.h",
    ],
    copts = [
        "-D__OPENCV_BUILD",
    ],
    deps = [
        ":all-indices",
        ":defines",
        ":dist",
        ":dummy",
        ":flann-base",
        ":general",
        ":index-testing",
        ":params",
        ":saving",
        "//third_party/cvlite/core",
    ],
)

cc_library(
    name = "nn-index",
    hdrs = [
        "nn-index.h",
    ],
    deps = [
        ":general",
        ":matrix",
        ":params",
        ":result-set",
    ],
)

cc_library(
    name = "params",
    hdrs = [
        "params.h",
    ],
    deps = [
        ":any",
        ":general",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "random",
    hdrs = [
        "random.h",
    ],
    deps = [
        ":general",
    ],
)

cc_library(
    name = "result-set",
    hdrs = [
        "result-set.h",
    ],
    deps = [
    ],
)

cc_library(
    name = "sampling",
    hdrs = [
        "sampling.h",
    ],
    deps = [
        ":matrix",
        ":random",
    ],
)

cc_library(
    name = "saving",
    hdrs = [
        "saving.h",
    ],
    deps = [
        ":general",
        ":nn-index",
        "//c8:exceptions",
    ],
)

cc_library(
    name = "timer",
    hdrs = [
        "timer.h",
    ],
    deps = [
        "//third_party/cvlite/core",
    ],
)
