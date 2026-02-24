"""
Used to bootstrap the webpack-build by transpiling the file using tsc directly.
"""

def _ts_bootstrap_impl(ctx):
    output_file = ctx.actions.declare_file("{}.js".format(ctx.label.name))

    shell_script = ctx.actions.declare_file("shell.sh")

    ctx.actions.write(
        output = shell_script,
        content = """
#!/bin/bash --norc
set -eu

absolute_node_modules=$(dirname $(realpath external/npm-webpack-build/fingerprint.sha1))/node_modules

echo '
{{
  "compilerOptions": {{
    "target": "esnext",
    "lib": ["esnext", "dom"],
    "module": "commonjs",
    "moduleResolution": "node",
    "noEmitOnError": true,
    "typeRoots": ["<absolute_node_modules>/@types"],
    "baseUrl": ".",
    "paths": {{
      "dts-bundle-generator": ["<absolute_node_modules>/dts-bundle-generator"],
      "webpack": ["<absolute_node_modules>/webpack"]
    }}
  }},
  "include": [
    "{input}"
  ]
}}
' | sed "s|<absolute_node_modules>|$absolute_node_modules|g" > tsconfig.json

{node_path} $absolute_node_modules/typescript/lib/tsc.js --project tsconfig.json

cp "{input_as_js}" "{output}"
""".format(
            node_path = ctx.file._node_bin.path,
            output = output_file.path,
            input = ctx.file.src.path,
            input_as_js = ctx.file.src.path.replace(".ts", ".js"),
        ),
        is_executable = True,
    )

    ctx.actions.run(
        inputs = [ctx.file.src, ctx.file._node_bin] + ctx.files._node_deps,
        outputs = [output_file],
        executable = shell_script,
    )

    return [DefaultInfo(files = depset([output_file]))]

ts_bootstrap = rule(
    attrs = {
        "src": attr.label(mandatory = True, allow_single_file = True),
        "_node_bin": attr.label(default = "@nodejs_host//:node_bin", allow_single_file = True),
        "_node_deps": attr.label(
            default = Label("@npm-webpack-build//:npm-webpack-build"),
            allow_files = True,
        ),
    },
    implementation = _ts_bootstrap_impl,
)
