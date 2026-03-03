"""module
It contains the main cuda_library macro to compile for cuda.

"""

load("@local_config_cuda//cuda:build_defs.bzl", _cuda_library = "cuda_library")
load("@v1-cuda-triplet//:defines.bzl", "CUDA_TRIPLET_ID", "CUDA_TRIPLET_ROOT")

def _check_cuda_impl(ctx):
    completion_token = ctx.actions.declare_file("%s" % (ctx.outputs.out.basename))

    ctx.actions.run(
        outputs = [completion_token],
        executable = ctx.executable._verify_cuda_env,
        arguments = [
            "{}/cuda/version.json".format(CUDA_TRIPLET_ROOT),
            completion_token.path,
        ] + (["system"] if ctx.attr.system else []),
        execution_requirements = {
            "no-cache": "1",
        },
    )

    return [
        DefaultInfo(files = depset([completion_token])),
        CcInfo(),
    ]

_chack_cuda = rule(
    implementation = _check_cuda_impl,
    attrs = {
        "out": attr.output(),
        "_verify_cuda_env": attr.label(
            default = "@the8thwall//bzl/gpu:verify_cuda_env",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
        "system": attr.bool(default = False),
    },
)

def cuda_library(
        name,
        copts = [],
        linkopts = [],
        hdrs = [],
        cuda_deps = [],
        deps = [],
        target_compatible_with = [],
        **kwargs):
    """Macro to compile for cuda libraries

    It behaves normally as cc_library with the different that clang is configured to compile for CUDA
    and the availabily of a valid cuda-triplet is verified before to compile.
    Read moduledoc at bzl/gpu/cuda-triplet.bzl for more information.

    Default shared libraries for cudnn and nccl are linked and used automatically.
    However you can override this by linking any of the library of the cuda triplet defined at
    bzl/thirdpartybuild/cuda-triplet.BUILD in the cuda_deps
    Args:
        name (str): name for this library.
        copts (list, optional): . Defaults to [].
        linkopts (list, optional): Additional compilation flags. Defaults to [].
        hdrs (list, optional): API headers for this library. Defaults to [].
        cuda_deps (list, optional): Depedencies on other cuda triplet libraries. Defaults to cudnn and nccl shared libraries.
        deps (list, optional): Depedencies on other bazel targets. Defaults to [].
        target_compatible_with (list, optional): List of condition this target is compatible with. Defaults to incompatible for all apple and cuda-support=none.
        **kwargs: All other possible flags,
    """

    verify_task_name = "{}_verify_cuda_env".format(name)
    dep_check_file = "{}.cuda_env.info.hpp".format(name)

    _final_target_compatible_with = select({
        "@the8thwall//bzl/conditions:cuda-incompatible": ["@platforms//:incompatible"],
        "//conditions:default": [],
    }) if not target_compatible_with else target_compatible_with

    _final_cuda_deps = [
        "@v1-cuda-triplet//:cudnn",
        "@v1-cuda-triplet//:nccl",
    ] if not cuda_deps else cuda_deps

    _chack_cuda(
        name = verify_task_name,
        out = dep_check_file,
        target_compatible_with = _final_target_compatible_with,
        system = select({
            "@the8thwall//bzl/conditions:cuda-system": True,
            "//conditions:default": False,
        }),
    )

    _cuda_library(
        name = name,
        copts = copts + [
            "--cuda-path=BAZEL_OUTPUT_BASE/external/{}/cuda".format(CUDA_TRIPLET_ID),
        ],
        linkopts = linkopts + [
            "--cuda-path=BAZEL_OUTPUT_BASE/external/{}/cuda".format(CUDA_TRIPLET_ID),
        ],
        deps = _final_cuda_deps + deps,
        hdrs = hdrs + [dep_check_file],
        target_compatible_with = _final_target_compatible_with,
        **kwargs
    )
