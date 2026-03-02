# TODO(mc): Encapsulate BIN_DIR directory as part of an llvm toolchain.
BIN_DIR = "Toolchains/LLVM15.0.0.xctoolchain/usr/bin"

def _llvm_tool_wrapper_impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name)
    llvm = ctx.toolchains["//bzl/llvm:toolchain_type"]

    llvm_bin = "{}/bin".format(llvm.llvminfo.usr)

    ctx.actions.write(
        output,
        content = """#!/bin/bash --norc
set -eu

export RUNFILES_DIR=${{RUNFILES_DIR:-$0.runfiles}}

WORKSPACE_ENV="{}/{}"

# Source BAZEL_OUTOUT_BASE.
. ${{RUNFILES_DIR}}/${{WORKSPACE_ENV}}

${{BAZEL_OUTPUT_BASE}}/{}/{} "$@"
""".format(ctx.workspace_name, ctx.file._workspace_env.path, llvm_bin, ctx.label.name),
        is_executable = True,
    )
    runfiles = ctx.runfiles([output, ctx.file._workspace_env])
    return [
        DefaultInfo(executable = output, files = depset([output]), runfiles = runfiles),
    ]

llvm_tool_wrapper = rule(
    implementation = _llvm_tool_wrapper_impl,
    attrs = {
        "_workspace_env": attr.label(
            default = "@workspace-env//:workspace-env",
            allow_single_file = True,
        ),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
    executable = True,
)
