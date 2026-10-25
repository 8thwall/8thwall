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
if command -v cygpath >/dev/null 2>&1; then
  absolute_node_modules=$(cygpath -m "$absolute_node_modules")
fi

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

compile_dir="$(dirname "{output}")/{output_basename}.bootstrap"
mkdir -p "$compile_dir"
trap 'rm -rf "$compile_dir"' EXIT

{node_path} $absolute_node_modules/typescript/lib/tsc.js --project tsconfig.json --outDir "$compile_dir"

cp "$compile_dir/{input_basename_as_js}" "{output}"
""".format(
            node_path = ctx.file._node_bin.path,
            output = output_file.path,
            output_basename = output_file.basename,
            input = ctx.file.src.path,
            input_basename_as_js = ctx.file.src.basename.replace(".ts", ".js"),
        ),
        is_executable = True,
    )

    action_executable = shell_script
    action_inputs = [ctx.file.src, ctx.file._node_bin, shell_script] + ctx.files._node_deps
    if ctx.configuration.host_path_separator == ";":
        batch_script = ctx.actions.declare_file("shell.cmd")
        ctx.actions.write(
            output = batch_script,
            content = """@echo off
set "BASH_EXE=%BAZEL_SH%"
if not defined BASH_EXE if exist "C:\\Program Files\\Git\\bin\\bash.exe" set "BASH_EXE=C:\\Program Files\\Git\\bin\\bash.exe"
if not defined BASH_EXE if exist "%ProgramFiles%\\Git\\bin\\bash.exe" set "BASH_EXE=%ProgramFiles%\\Git\\bin\\bash.exe"
if not defined BASH_EXE if exist "%LocalAppData%\\Programs\\Git\\bin\\bash.exe" set "BASH_EXE=%LocalAppData%\\Programs\\Git\\bin\\bash.exe"
if not defined BASH_EXE (
  echo Git Bash was not found. Set BAZEL_SH to its bash.exe path. 1>&2
  exit /b 1
)
"%BASH_EXE%" "%~dp0shell.sh"
""",
            is_executable = True,
        )
        action_executable = batch_script
        action_inputs.append(batch_script)

    ctx.actions.run(
        inputs = action_inputs,
        outputs = [output_file],
        executable = action_executable,
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
