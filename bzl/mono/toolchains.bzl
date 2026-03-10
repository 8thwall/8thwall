MonoInfo = provider(
    doc = "Information about how to invoke the Mono (.NET) toolchain.",
    fields = ["mono_path"],
)

def _mono_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        monoinfo = MonoInfo(
            mono_path = ctx.attr.mono.label.workspace_root,
        ),
    )
    return [toolchain_info]

mono_toolchain = rule(
    implementation = _mono_toolchain_impl,
    attrs = {
        "mono": attr.label(mandatory = True),
    },
)
