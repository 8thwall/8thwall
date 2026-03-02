"""Rule for zip.
   
   merge - combine multiple .zip files together
"""

def _merge_impl(ctx):
    output_file = ctx.actions.declare_file("{}.zip".format(ctx.label.name))
    ctx.actions.run_shell(
        inputs = ctx.files.srcs,
        outputs = [output_file],
        command = """
set -e
mkdir _files
echo {srcs} | xargs -n 1 unzip -qq -d _files -o
cd _files
# NOTE(christoph): This prevents date modified timestamps from changing the output zip
find . -exec touch -t 198001010000 {exec_placeholder} +
zip -qqr ../{output_path} .
""".format(
            exec_placeholder = "{}",
            srcs = " ".join([f.path for f in ctx.files.srcs]),
            output_path = output_file.path,
        ),
    )

    return [DefaultInfo(files = depset([output_file]))]

merge_zip = rule(
    implementation = _merge_impl,
    attrs = {
        "srcs": attr.label_list(mandatory = True, allow_files = True),
    },
)
