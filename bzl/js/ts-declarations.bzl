load("//bzl/js:js.bzl", "js_files_provider")

def _ts_declarations_impl(ctx):
    declaration_files = [src for src in ctx.attr.binary[js_files_provider].transitive_srcs.to_list() if src.path.endswith(".d.ts")]

    return [
        DefaultInfo(files = depset(declaration_files)),
        js_files_provider(
            transitive_srcs = depset(declaration_files),
            includes = depset([src.root.path for src in declaration_files]),
            runfiles = ctx.runfiles([]),
        ),
    ]

ts_declarations = rule(
    implementation = _ts_declarations_impl,
    attrs = {
        "binary": attr.label(
            allow_files = True,
        ),
    },
)
