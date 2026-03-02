licenses(["permissive"])  # MPL2

package(default_visibility = ["//visibility:public"])

alias(
    name = "eigen",
    actual = ":eigen3",
)

cc_library(
    name = "eigen3",
    hdrs = glob([
        "**/*.h",
        "unsupported/Eigen/*",
        "unsupported/Eigen/CXX11/*",
        "Eigen/*",
    ]),
    copts = [
        "-DEIGEN_MPL2_ONLY",  # will only include MPL2 or more permissive code
    ],
    # Add these to match mem alignment of Eigen in TensorFlow
    defines = [
        "EIGEN_MPL2_ONLY",
        "EIGEN_MAX_ALIGN_BYTES=64",
    ],
    includes = [
        ".",
        "unsupported",
    ],
    visibility = ["//visibility:public"],
)
