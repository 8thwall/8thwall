"""Rule for zip.
   
   unzip - Unzips a set of named output files from a .zip file.
"""

def _unzip_impl(ctx):
    outputs = [ctx.actions.declare_file(out) for out in ctx.attr.outs]
    output_directory = "{}/{}".format(ctx.bin_dir.path, ctx.label.package)
    ctx.actions.run_shell(
        outputs = outputs,
        inputs = ctx.files.srcs,
        command = "unzip -qq {files} -d {output_dir}".format(files = " ".join([src.path for src in ctx.files.srcs]), output_dir = output_directory),
        arguments = [
            "-qq",
        ] + [src.path for src in ctx.files.srcs] + [
            "-d",
            output_directory,
        ],
        mnemonic = "Unzip",
        progress_message = "Unzipping %{input}",
    )

    return [
        DefaultInfo(files = depset(outputs), runfiles = ctx.runfiles(outputs)),
    ]

unzip = rule(
    implementation = _unzip_impl,
    attrs = {
        # List of .zip files to unzip.
        "srcs": attr.label_list(mandatory = True, allow_files = True),
        # Files that will be expected from this extraction. This is a
        # string_list instead of an output_list because the latter are not
        # configurable with selects.
        "outs": attr.string_list(mandatory = True),
    },
)
