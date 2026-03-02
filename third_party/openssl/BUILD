load("@the8thwall//bzl/cmake:cmake-cc-wrapper.bzl", "cmake_cc_wrapper")

package(default_visibility = ["//visibility:public"])

exports_files(["Configure.patch"])

filegroup(
    name = "openssl-build",
    srcs = ["openssl-build.sh"],
)

cmake_cc_wrapper(
    name = "openssl-crypto-cmake",
    package_name = "crypto",
    dep = "@openssl//:crypto",
    visibility = ["//visibility:public"],
)
