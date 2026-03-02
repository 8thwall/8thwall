def _node_toolchain_impl(repository_ctx):
    repository_ctx.symlink("../node-14-macosx", "node-install")
    print(repository_ctx.attr.name)
    print(repository_ctx.os.name)
    print(repository_ctx.os.arch)

node_toolchain = repository_rule(
    implementation = _node_toolchain_impl,
    attrs = {},
)
