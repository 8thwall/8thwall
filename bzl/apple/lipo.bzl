CcInfoArrayInfo = provider(
    "Pass information about dependent files in an xcode project.",
    fields = {
        "ccinfo": "array of CcInfo from dependent files",
    },
)

def _lipo_impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name)
    ctx.actions.run(
        inputs = ctx.files.binaries,
        outputs = [out],
        executable = ctx.file._lipo_tool,
        arguments = [f.path for f in ctx.files.binaries] + ["-create", "-output", out.path],
    )

    return [
        DefaultInfo(
            executable = out,
            files = depset([out]),
        ),
        CcInfoArrayInfo(
            ccinfo = [binary[CcInfo] for binary in ctx.attr.binaries],
        ),
    ]

# Create a multi-architecture binary by using the 'lipo' tool to combine the output from a set of cc_binary targets
lipo_binary = rule(
    implementation = _lipo_impl,
    attrs = {
        "binaries": attr.label_list(
            providers = [CcInfo],
        ),
        "_lipo_tool": attr.label(
            default = "//bzl/llvm:llvm-lipo",
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
