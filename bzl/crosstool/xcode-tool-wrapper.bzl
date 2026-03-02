def _xcode_tool_wrapper_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".sh")
    llvminfo = ctx.toolchains["//bzl/llvm:toolchain_type"].llvminfo
    ctx.actions.expand_template(
        output = out,
        template = ctx.file.template,
        substitutions = {
            "{{WORKSPACE}}": ctx.workspace_name,
            "{{WORKSPACE_ENV}}": ctx.file._workspace_env.path,
            "{{XCTOOLCHAIN}}": llvminfo.xcode_xctoolchain,
            "{{RELATIVE_EXTERNAL_TOOLCHAINS_DIR}}": "{}/Toolchains".format(llvminfo.root),
        },
        is_executable = True,
    )
    return [DefaultInfo(executable = out, files = depset([out]), runfiles = ctx.runfiles([out], transitive_files = depset(ctx.files.data)))]

xcode_tool_wrapper = rule(
    implementation = _xcode_tool_wrapper_impl,
    executable = True,
    attrs = {
        "_workspace_env": attr.label(default = "@workspace-env//:workspace-env", allow_single_file = True),
        "template": attr.label(
            allow_single_file = [".sh.tpl"],
            mandatory = True,
        ),
        "data": attr.label_list(
            allow_files = True,
            default = [],
        ),
    },
    toolchains = [
        "//bzl/llvm:toolchain_type",
    ],
)
