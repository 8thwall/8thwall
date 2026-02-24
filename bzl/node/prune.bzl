def _npm_prune_impl(ctx):
    output_file = ctx.actions.declare_file("{}.zip".format(ctx.label.name))

    ctx.actions.run_shell(
        inputs = [ctx.file.modules],
        outputs = [output_file],
        tools = [ctx.file._npm],
        command = """
set -e
mkdir _files
unzip -qq {zip_path} -d _files
cd _files
export npm_config_cache='.npm'
../{npm} prune --production 2> /dev/null > /dev/null 
# NOTE(christoph): This prevents date modified timestamps from changing the output zip
find node_modules -exec touch -t 198001010000 {exec_placeholder} +
zip -qqr ../{output_file} node_modules
""".format(
            exec_placeholder = "{}",
            npm = ctx.file._npm.path,
            zip_path = ctx.file.modules.path,
            output_file = output_file.path,
        ),
    )

    return [DefaultInfo(files = depset([output_file]))]

npm_prune = rule(
    implementation = _npm_prune_impl,
    attrs = {
        "_npm": attr.label(default = "//bzl/node:npm", allow_single_file = True, executable = True, cfg = "exec"),
        "modules": attr.label(allow_single_file = True),
    },
)
