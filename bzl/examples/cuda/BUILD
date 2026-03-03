load("//bzl/gpu:cuda.bzl", "cuda_library")

# NOTE:
# 1. This macro makes the CUDA libraries linking cudnn and nccl shared libraries of the cuda triplet
#    by default. You can override this by passing cuda_deps arguments with any of the one at
#    bzl/thirdpartybuild/cuda-triplet.BUILD.
#    Read moduledoc at bzl/gpu/cuda-triplet.bzl for more information about the cuda triplet.
# 2. This macro by default is incompatible with all apple targets and -//bzl/gpu:cuda-support=none
#    You can override this by specify target_compatible_with flags (if you really know what you are doing)

cuda_library(
    name = "exampleVectorAdd",
    srcs = ["vectorAdd.cu"],
    hdrs = ["vectorAdd.h"],
    visibility = ["//visibility:public"],
)

cuda_target_compatible_with = select({
    "@the8thwall//bzl/conditions:cuda-incompatible": ["@platforms//:incompatible"],
    "//conditions:default": [],
})

cc_binary(
    name = "exampleVectorAdd_exec",
    srcs = ["main.cpp"],
    linkstatic = 0,
    target_compatible_with = cuda_target_compatible_with,
    deps = [
        ":exampleVectorAdd",
        "@v1-cuda-triplet//:cudart",
    ],
)

cc_binary(
    name = "exampleVectorAdd_exec_static",
    srcs = ["main.cpp"],
    linkopts = [
        "-lpthread",
        "-lrt",
        "-ldl",
    ],
    linkstatic = 1,
    target_compatible_with = cuda_target_compatible_with,
    deps = [
        ":exampleVectorAdd",
        "@v1-cuda-triplet//:cudart_static",
    ],
)
