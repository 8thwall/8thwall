load("@bazel-contrib-rules_cuda//cuda:defs.bzl", "cuda_library")

package(default_visibility = ["//visibility:public"])

cuda_target_compatible_with = select({
    "@the8thwall//bzl/conditions:linux": [],
    "//conditions:default": ["@platforms//:incompatible"],
})

cuda_library(
    name = "kernel",
    srcs = ["kernel.cu"],
    hdrs = ["kernel.h"],
    target_compatible_with = cuda_target_compatible_with,
)

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    target_compatible_with = cuda_target_compatible_with,
    deps = [":kernel"],
)

cc_binary(
    name = "cublas_main",
    srcs = ["cublas.cpp"],
    target_compatible_with = cuda_target_compatible_with,
    deps = ["@v1-cuda-triplet//:cublas"],
)
