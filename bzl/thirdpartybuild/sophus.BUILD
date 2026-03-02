licenses(["notice"])  # MIT License

package(default_visibility = ["//visibility:public"])

SOPHUS_CERES_HEADER_FILES = [
    "sophus/ceres_local_parameterization.hpp",
    "sophus/ceres_manifold.hpp",
    "sophus/ceres_typetraits.hpp",
]

SOPHUS_HEADER_FILES = [
    "sophus/average.hpp",
    "sophus/cartesian.hpp",
    "sophus/common.hpp",
    "sophus/geometry.hpp",
    "sophus/interpolate.hpp",
    "sophus/interpolate_details.hpp",
    "sophus/num_diff.hpp",
    "sophus/rotation_matrix.hpp",
    "sophus/rxso2.hpp",
    "sophus/rxso3.hpp",
    "sophus/se2.hpp",
    "sophus/se3.hpp",
    "sophus/sim2.hpp",
    "sophus/sim3.hpp",
    "sophus/sim_details.hpp",
    "sophus/so2.hpp",
    "sophus/so3.hpp",
    "sophus/spline.hpp",
    "sophus/types.hpp",
    "sophus/velocities.hpp",
]

SOPHUS_OTHER_HEADER_FILES = [
    "sophus/test_macros.hpp",
]

SOPHUS_OTHER_SOURCE_FILES = [
    "sophus/example_ensure_handler.cpp",
]

cc_library(
    name = "sophus",
    srcs = SOPHUS_OTHER_SOURCE_FILES,
    hdrs = SOPHUS_HEADER_FILES + SOPHUS_CERES_HEADER_FILES + SOPHUS_OTHER_HEADER_FILES,
    defines = ["SOPHUS_USE_BASIC_LOGGING"],
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "@eigen3",
    ],
)

cc_test(
    name = "test_common",
    srcs = [
        "test/core/test_common.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_cartesian2",
    srcs = [
        "test/core/test_cartesian2.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_cartesian3",
    srcs = [
        "test/core/test_cartesian3.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_so2",
    srcs = [
        "test/core/test_so2.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_se2",
    srcs = [
        "test/core/test_se2.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_rxso2",
    srcs = [
        "test/core/test_rxso2.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_sim2",
    srcs = [
        "test/core/test_sim2.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_so3",
    srcs = [
        "test/core/test_so3.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_se3",
    srcs = [
        "test/core/test_se3.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_rxso3",
    srcs = [
        "test/core/test_rxso3.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_sim3",
    srcs = [
        "test/core/test_sim3.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_velocities",
    srcs = [
        "test/core/test_velocities.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_geometry",
    srcs = [
        "test/core/test_geometry.cpp",
        "test/core/tests.hpp",
    ],
    deps = [
        ":sophus",
    ],
)

cc_test(
    name = "test_ceres_so3",
    srcs = [
        "test/ceres/test_ceres_so3.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_rxso3",
    srcs = [
        "test/ceres/test_ceres_rxso3.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_se3",
    srcs = [
        "test/ceres/test_ceres_se3.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_sim3",
    srcs = [
        "test/ceres/test_ceres_sim3.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_so2",
    srcs = [
        "test/ceres/test_ceres_so2.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_rxso2",
    srcs = [
        "test/ceres/test_ceres_rxso2.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_se2",
    srcs = [
        "test/ceres/test_ceres_se2.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)

cc_test(
    name = "test_ceres_sim2",
    srcs = [
        "test/ceres/test_ceres_sim2.cpp",
        "test/ceres/tests.hpp",
    ],
    deps = [
        ":sophus",
        "@ceres2//:ceres",
    ],
)
