load("//bzl/js:js.bzl", "js_files_provider")

def _impl(ctx):
    out = ctx.actions.declare_file(ctx.label.name + ".js")
    ctx.actions.run_shell(
        inputs = [ctx.file.template, ctx.file.version],
        outputs = [out],
        command = "cat '%s' | sed \"s|XR8_VERSION|$(cat '%s')|g\" | sed \"s|XR8_IS_SIMD|%s|g\" > '%s'" % (
            ctx.file.template.path,
            ctx.file.version.path,
            "true" if ctx.attr.simd else "false",
            out.path,
        ),
    )

    return [
        DefaultInfo(files = depset([out])),
        js_files_provider(transitive_srcs = depset([out]), runfiles = ctx.runfiles([]), includes = depset([ctx.bin_dir.path])),
    ]

xr_js_config = rule(
    implementation = _impl,
    attrs = {
        "template": attr.label(mandatory = True, allow_single_file = True),
        "simd": attr.bool(mandatory = True),
        "version": attr.label(mandatory = True, allow_single_file = True),
    },
)
