licenses(["permissive"])  # MPL2

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "eigen",
    hdrs = glob([
        "**/*.h",
        "unsupported/Eigen/*",
        "unsupported/Eigen/CXX11/*",
        "Eigen/*",
    ]),
    copts = [
        "-DEIGEN_MPL2_ONLY",  # will only include MPL2 or more permissive code
    ],
    includes = [
        ".",
        "unsupported",
    ],
    visibility = ["//visibility:public"],
)
