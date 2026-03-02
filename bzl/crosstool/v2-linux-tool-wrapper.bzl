"""module
Tool wrapper utils for v1-linux
"""

# The Bazel strip action uses the default shell env so it won't accept the environment variables we
# pass in, so we need to manually add the toolchain env vars.
def _v2_linux_tool_wrapper_impl(ctx):
    llvminfo = ctx.toolchains["//bzl/llvm:toolchain_type"].llvminfo

    out = ctx.actions.declare_file(ctx.label.name + ".sh")
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "{{WORKSPACE_ENV}}": ctx.file._workspace_env.path,
            "{{LLVM_USR}}": llvminfo.usr,
        },
        is_executable = True,
    )
    return [DefaultInfo(executable = out, files = depset([out]), runfiles = ctx.runfiles([out]))]

v2_linux_tool_wrapper = rule(
    implementation = _v2_linux_tool_wrapper_impl,
    executable = True,
    attrs = {
        "_workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "template": attr.label(
            allow_single_file = [".sh.tpl"],
            mandatory = True,
        ),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
)
