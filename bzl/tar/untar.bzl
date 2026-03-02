"""
Extracts a .tar file to a directory.
"""

def _untar_impl(ctx):
    outdir = ctx.actions.declare_directory(ctx.label.name)

    ctx.actions.run_shell(
        outputs = [outdir],
        inputs = [ctx.file.src],
        command = "tar -xzf {src} -C {outdir}".format(
            src = ctx.file.src.path,
            outdir = outdir.path,
        ),
        mnemonic = "Untar",
        progress_message = "Extracting {}".format(ctx.file.src.path),
    )

    return [
        DefaultInfo(files = depset([outdir]), runfiles = ctx.runfiles([outdir])),
    ]

untar = rule(
    implementation = _untar_impl,
    attrs = {
        "src": attr.label(mandatory = True, allow_single_file = True),
    },
)
