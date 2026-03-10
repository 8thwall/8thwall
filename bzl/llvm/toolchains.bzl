LlvmInfo = provider(
    doc = "Information about how to invoke the LLVM toolchain.",
    fields = [
        "root",
        "usr",
        "runtime",
        "xcode_xctoolchain",
        "xcode_toolchain_id",
    ],
)

def _llvm_toolchain_impl(ctx):
    root = ctx.attr.llvm.label.workspace_root
    toolchain_info = platform_common.ToolchainInfo(
        llvminfo = LlvmInfo(
            root = root,
            usr = "{}/{}".format(root, ctx.attr.usr) if ctx.attr.usr else root,
            runtime = "{}/{}".format(root, ctx.attr.runtime),
            xcode_xctoolchain = ctx.attr.xcode_xctoolchain,
            xcode_toolchain_id = ctx.attr.xcode_toolchain_id,
        ),
    )
    return [toolchain_info]

llvm_toolchain = rule(
    implementation = _llvm_toolchain_impl,
    attrs = {
        # Label to the llvm stub.
        "llvm": attr.label(mandatory = True),
        # Relative path to the 'usr' directory.
        "usr": attr.string(default = ""),
        # Relative path to the runtime directory, e.g. 'usr/lib/clang/15.0.0'.
        "runtime": attr.string(mandatory = True),
        # Name of the xctoolchain directory, e.g. 'LLVM15.0.0.xctoolchain', if this is an XCode-compatible LLVM.
        "xcode_xctoolchain": attr.string(default = ""),
        # Name of the XCode toolchain ID is an XCode-compatible LLVM, e.g. 'org.llvm.15.0.0'. This is
        # found in the Info.plist in the toolchain.
        "xcode_toolchain_id": attr.string(default = ""),
    },
)
