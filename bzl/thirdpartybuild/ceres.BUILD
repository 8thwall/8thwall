licenses(["permissive"])  # BSD

load("@the8thwall//bzl/crosstool:simd.bzl", "SIMD")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "miniglog",
    srcs =
        [
            "config/ceres/internal/config.h",
            "include/ceres/internal/disable_warnings.h",
            "include/ceres/internal/port.h",
            "include/ceres/internal/reenable_warnings.h",
            "internal/ceres/miniglog/glog/logging.cc",
        ],
    hdrs =
        [
            "internal/ceres/miniglog/glog/logging.h",
        ],
    # This needs to be in a defines block, since any code that includes
    # the 'port.h' header will fail without it and ceres is a template library.
    defines = select({
        "@the8thwall//bzl/conditions:linux": [
            # Only one of the following may be defined
            "CERES_USE_CXX_THREADS",  # C++ Standard Library threads

            # Only use the following #define if OpenMP is made available in the
            # Linux CC toolchains
            # "CERES_USE_OPENMP",
        ],
        "//conditions:default": [
            "CERES_NO_THREADS",
        ],
    }),
    includes =
        [
            "config",
            "include",
            "internal/ceres/miniglog",
        ],
    visibility = ["//visibility:public"],
)

ceres_cc_threading_srcs = select({
    "@the8thwall//bzl/conditions:linux": [
        # Only one of the following srcs may be defined
        "internal/ceres/parallel_for_cxx.cc",

        # Not currently used, the existing hermetic Linux CC toolchains do not
        # have OpenMP support enabled
        # "internal/ceres/parallel_for_openmp.cc",
    ],
    "//conditions:default": [
        "internal/ceres/parallel_for_nothreads.cc",
    ],
})

cc_library(
    name = "ceres",
    srcs = ceres_cc_threading_srcs + glob(
        [
            "config/ceres/internal/*.h",
            "include/ceres/internal/*.h",
            "internal/ceres/*.h",
            "internal/ceres/generated/*.cc",
        ],
        exclude =
            [
                "internal/ceres/*_test_utils.cc",
                "internal/ceres/*_test.cc",
                "internal/ceres/gmock_main.cc",
                "internal/ceres/gmock_gtest_all.cc",
                "internal/ceres/test_util.cc",
                "internal/ceres/test_util.cc",
            ],
    ) + [
        "internal/ceres/array_utils.cc",
        "internal/ceres/blas.cc",
        "internal/ceres/block_evaluate_preparer.cc",
        "internal/ceres/block_jacobi_preconditioner.cc",
        "internal/ceres/block_jacobian_writer.cc",
        "internal/ceres/block_random_access_dense_matrix.cc",
        "internal/ceres/block_random_access_diagonal_matrix.cc",
        "internal/ceres/block_random_access_matrix.cc",
        "internal/ceres/block_random_access_sparse_matrix.cc",
        "internal/ceres/block_sparse_matrix.cc",
        "internal/ceres/block_structure.cc",
        "internal/ceres/c_api.cc",
        "internal/ceres/callbacks.cc",
        "internal/ceres/canonical_views_clustering.cc",
        "internal/ceres/cgnr_solver.cc",
        "internal/ceres/compressed_col_sparse_matrix_utils.cc",
        "internal/ceres/compressed_row_jacobian_writer.cc",
        "internal/ceres/compressed_row_sparse_matrix.cc",
        "internal/ceres/conditioned_cost_function.cc",
        "internal/ceres/conjugate_gradients_solver.cc",
        "internal/ceres/context.cc",
        "internal/ceres/context_impl.cc",
        "internal/ceres/coordinate_descent_minimizer.cc",
        "internal/ceres/corrector.cc",
        "internal/ceres/covariance.cc",
        "internal/ceres/covariance_impl.cc",
        "internal/ceres/cxsparse.cc",
        "internal/ceres/dense_normal_cholesky_solver.cc",
        "internal/ceres/dense_qr_solver.cc",
        "internal/ceres/dense_sparse_matrix.cc",
        "internal/ceres/detect_structure.cc",
        "internal/ceres/dogleg_strategy.cc",
        "internal/ceres/dynamic_compressed_row_jacobian_writer.cc",
        "internal/ceres/dynamic_compressed_row_sparse_matrix.cc",
        "internal/ceres/dynamic_sparse_normal_cholesky_solver.cc",
        "internal/ceres/eigensparse.cc",
        "internal/ceres/evaluator.cc",
        "internal/ceres/file.cc",
        "internal/ceres/function_sample.cc",
        "internal/ceres/gradient_checker.cc",
        "internal/ceres/gradient_checking_cost_function.cc",
        "internal/ceres/gradient_problem.cc",
        "internal/ceres/gradient_problem_solver.cc",
        "internal/ceres/implicit_schur_complement.cc",
        "internal/ceres/inner_product_computer.cc",
        "internal/ceres/is_close.cc",
        "internal/ceres/iterative_refiner.cc",
        "internal/ceres/iterative_schur_complement_solver.cc",
        "internal/ceres/lapack.cc",
        "internal/ceres/levenberg_marquardt_strategy.cc",
        "internal/ceres/line_search.cc",
        "internal/ceres/line_search_direction.cc",
        "internal/ceres/line_search_minimizer.cc",
        "internal/ceres/line_search_preprocessor.cc",
        "internal/ceres/linear_least_squares_problems.cc",
        "internal/ceres/linear_operator.cc",
        "internal/ceres/linear_solver.cc",
        "internal/ceres/local_parameterization.cc",
        "internal/ceres/loss_function.cc",
        "internal/ceres/low_rank_inverse_hessian.cc",
        "internal/ceres/minimizer.cc",
        "internal/ceres/normal_prior.cc",
        "internal/ceres/parallel_utils.cc",
        "internal/ceres/parameter_block_ordering.cc",
        "internal/ceres/partitioned_matrix_view.cc",
        "internal/ceres/polynomial.cc",
        "internal/ceres/preconditioner.cc",
        "internal/ceres/preprocessor.cc",
        "internal/ceres/problem.cc",
        "internal/ceres/problem_impl.cc",
        "internal/ceres/program.cc",
        "internal/ceres/reorder_program.cc",
        "internal/ceres/residual_block.cc",
        "internal/ceres/residual_block_utils.cc",
        "internal/ceres/schur_complement_solver.cc",
        "internal/ceres/schur_eliminator.cc",
        "internal/ceres/schur_jacobi_preconditioner.cc",
        "internal/ceres/schur_templates.cc",
        "internal/ceres/scratch_evaluate_preparer.cc",
        "internal/ceres/single_linkage_clustering.cc",
        "internal/ceres/solver.cc",
        "internal/ceres/solver_utils.cc",
        "internal/ceres/sparse_cholesky.cc",
        "internal/ceres/sparse_matrix.cc",
        "internal/ceres/sparse_normal_cholesky_solver.cc",
        "internal/ceres/split.cc",
        "internal/ceres/stringprintf.cc",
        "internal/ceres/subset_preconditioner.cc",
        "internal/ceres/suitesparse.cc",
        "internal/ceres/thread_pool.cc",
        "internal/ceres/thread_token_provider.cc",
        "internal/ceres/triplet_sparse_matrix.cc",
        "internal/ceres/trust_region_minimizer.cc",
        "internal/ceres/trust_region_preprocessor.cc",
        "internal/ceres/trust_region_step_evaluator.cc",
        "internal/ceres/trust_region_strategy.cc",
        "internal/ceres/types.cc",
        "internal/ceres/visibility.cc",
        "internal/ceres/visibility_based_preconditioner.cc",
        "internal/ceres/wall_time.cc",
    ],
    hdrs = glob(["include/ceres/*.h"]),
    copts =
        SIMD.SSE4_2 + [
            # NOTE: -DCERES_NO_THREADS is pulled in through miniglob above.
            "-Dmix=ceres::internal::hash_mix",
            "-DMAX_LOG_LEVEL=-2",
        ],
    defines = [
        "CERES_STD_UNORDERED_MAP",
        "CERES_NO_SUITESPARSE",
        "CERES_NO_LAPACK",
        "CERES_NO_CXSPARSE",
        "CERES_NO_ACCELERATE_SPARSE",
        "CERES_USE_EIGEN_SPARSE",
        "CERES_HAVE_PTHREAD",
    ],
    includes =
        [
            "",
            "config",
            "include",
            "internal",
        ],
    visibility = ["//visibility:public"],
    deps =
        [
            ":miniglog",
            "@eigen3",
        ],
)
