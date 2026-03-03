# During the analysis phase, toolchain resolution occurs whether or not a target
# is compatible with the specified target platform.
# When there is no match from the platform -> toolchain, the build fails with a
# "no matching toolchains found" error before the `target_compatible_with` directive
# can omit the target.
#
# This is a known issue[1,2] that was addressed in Bazel 6.0[3]. However as of
# Fri 07 Apr 2023, `repo/niantic` still uses Bazel 5.4.
#
# Until `repo/niantic` upgrades to Bazel 6.0, what we instead do is define a dummy
# CUDA toolchain for non-Linux platforms to allow toolchain resolution to succeed
# when building for non-Linux, but then continue to rely on `target_compatible_with`
# to then omit incompatible targets from the execution phase.
#
# Links:
# [1] https://groups.google.com/g/bazel-discuss/c/mHaQfcUZ4Vw/m/XDwPDnusAAAJ
# [2] https://github.com/bazelbuild/proposals/blob/main/designs/2022-01-21-optional-toolchains.md
# [3] https://blog.bazel.build/2022/12/19/bazel-6.0.html#optional-toolchains

def _dummy_toolchain_impl(ctx):
    return platform_common.ToolchainInfo()

_dummy_toolchain = rule(
    implementation = _dummy_toolchain_impl,
)

toolchain_name = "dummy-cuda-toolchain"

def create_dummy_cuda_toolchain():
    _dummy_toolchain(
        name = "dummy",
    )

    native.toolchain(
        name = toolchain_name,
        toolchain = ":dummy",
        toolchain_type = "@bazel-contrib-rules_cuda//cuda:toolchain_type",
        visibility = ["//visibility:public"],
    )

def register_dummy_cuda_toolchain():
    """Helper to register automatically generated placeholder CUDA toolchain(s).
"""
    toolchain_label = "//bzl/thirdpartybuild/bazel-contrib-rules_cuda:{}".format(toolchain_name)
    native.register_toolchains(toolchain_label)
