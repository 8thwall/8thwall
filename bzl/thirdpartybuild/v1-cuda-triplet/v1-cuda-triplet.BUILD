load("@the8thwall//bzl/thirdpartybuild/v1-cuda-triplet:v1-cuda-triplet.bzl", "cuda_prebuilt_library")
# This stub rules allows the external repository to load, without adding files
# to the bazel sandbox.

# v1-cuda-triplet
# ├── cuda -> cuda_11.2.2
# ├── cuda_11.2.2
# ├── cudnn -> cudnn-linux-x86_64-8.7.0.84_cuda11-archive
# ├── cudnn-linux-x86_64-8.7.0.84_cuda11-archive
# ├── nccl -> nccl_2.16.5-1+cuda11.0_x86_64
# └── nccl_2.16.5-1+cuda11.0_x86_64

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "stub",
    srcs = [],
)

filegroup(
    name = "all-files",
    srcs = glob(["**/*"]),
)

# Most of the cuda libraries have SONAME redirected to versioned library,
# so we need to instruct bazel to included the linked version in the wrapped imported libraries
# For more information, see the following links:
#
# - https://github.com/bazelbuild/bazel/issues/1534
# - https://stackoverflow.com/questions/68405534/link-to-versioned-pre-built-libraries-with-bazel

# Wrapped cuda libraries (static and shared library available): shared
CUDA_LIBS = [
    "cublas",
    "cublasLt",
    "cudart",
    "cufft",
    "cufftw",
    "curand",
    "cusolver",
    "cusparse",
    "nppc",
    "nppial",
    "nppicc",
    "nppidei",
    "nppif",
    "nppig",
    "nppim",
    "nppist",
    "nppisu",
    "nppitc",
    "npps",
    "nvjpeg",
    "nvrtc-builtins",
    "nvrtc",
    "OpenCL",
    "accinj64",
    "cuinj64",
    "nvToolsExt",
    "nvblas",
    "cusolverMg",
]

[
    cuda_prebuilt_library(
        name = lib_name,
        hdrs = glob([
            "cuda/include/**/*.h",
            "cuda/include/**/*.hpp",
        ]),
        includes = ["cuda/include"],
        path_prefix = "cuda/lib64",
    )
    for lib_name in CUDA_LIBS
]

# Wrapped cuda libraries (only static library available)
cc_library(
    name = "cudadevrt",
    srcs = ["cuda/lib64/libcudadevrt.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

cc_library(
    name = "cufilt",
    srcs = ["cuda/lib64/libcufilt.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

cc_library(
    name = "culibos",
    srcs = ["cuda/lib64/libculibos.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

cc_library(
    name = "lapack_static",
    srcs = ["cuda/lib64/liblapack_static.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

cc_library(
    name = "metis_static",
    srcs = ["cuda/lib64/libmetis_static.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

cc_library(
    name = "nvptxcompiler_static",
    srcs = ["cuda/lib64/libnvptxcompiler_static.a"],
    hdrs = glob([
        "cuda/include/**/*.h",
    ]),
    includes = ["cuda/include"],
)

# Wrapped nccl libraries
cc_library(
    name = "nccl",
    srcs = [
        "nccl/lib/libnccl.so",
        "nccl/lib/libnccl.so.2",
    ],
    hdrs = glob([
        "nccl/**/*.h",
    ]),
    includes = ["nccl/include"],
)

cc_library(
    name = "nccl_static",
    srcs = ["nccl/lib/libnccl_static.a"],
    hdrs = glob([
        "nccl/**/*.h",
    ]),
    includes = ["nccl/include"],
)

# Wraped cudnn library
cc_library(
    name = "cudnn",
    srcs = [
        "cudnn/lib/libcudnn.so",
        "cudnn/lib/libcudnn.so.8",
    ],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_infer",
    srcs = ["cudnn/lib/libcudnn_adv_infer.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_infer_static",
    srcs = ["cudnn/lib/libcudnn_adv_infer_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_infer_static_v8",
    srcs = ["cudnn/lib/libcudnn_adv_infer_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_train",
    srcs = ["cudnn/lib/libcudnn_adv_train.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_train_static",
    srcs = ["cudnn/lib/libcudnn_adv_train_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_adv_train_static_v8",
    srcs = ["cudnn/lib/libcudnn_adv_train_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_infer",
    srcs = ["cudnn/lib/libcudnn_cnn_infer.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_infer_static",
    srcs = ["cudnn/lib/libcudnn_cnn_infer_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_infer_static_v8",
    srcs = ["cudnn/lib/libcudnn_cnn_infer_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_train",
    srcs = ["cudnn/lib/libcudnn_cnn_train.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_train_static",
    srcs = ["cudnn/lib/libcudnn_cnn_train_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_cnn_train_static_v8",
    srcs = ["cudnn/lib/libcudnn_cnn_train_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_infer",
    srcs = ["cudnn/lib/libcudnn_ops_infer.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_infer_static",
    srcs = ["cudnn/lib/libcudnn_ops_infer_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_infer_static_v8",
    srcs = ["cudnn/lib/libcudnn_ops_infer_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_train",
    srcs = ["cudnn/lib/libcudnn_ops_train.so"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_train_static",
    srcs = ["cudnn/lib/libcudnn_ops_train_static.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)

cc_library(
    name = "cudnn_ops_train_static_v8",
    srcs = ["cudnn/lib/libcudnn_ops_train_static_v8.a"],
    hdrs = glob([
        "cudnn/**/*.h",
    ]),
    includes = ["cudnn/include"],
)
