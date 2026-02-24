load("//bzl/js:js.bzl", "js_files_provider")

def _js_stringloader_impl(ctx):
    js_out = ctx.actions.declare_file("{}.js".format(ctx.label.name))

    inputs = ctx.files.srcs

    ctx.actions.run(
        inputs = inputs,
        outputs = [js_out],
        executable = ctx.executable._file_loader,
        arguments = [
            js_out.path,
        ] + [f.path for f in ctx.files.srcs],
    )

    return [
        DefaultInfo(files = depset([js_out])),
        js_files_provider(
            transitive_srcs = depset([js_out]),
            runfiles = ctx.runfiles(),
            includes = depset([js_out.root.path]),
        ),
    ]

js_stringloader = rule(
    implementation = _js_stringloader_impl,
    attrs = {
        "srcs": attr.label_list(
            default = [],
            allow_files = True,
        ),
        "_file_loader": attr.label(default = ":fileloader", cfg = "exec", allow_single_file = True, executable = True),
    },
)
