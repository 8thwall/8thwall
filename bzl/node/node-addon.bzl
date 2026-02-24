"""
This module provides a Bazel rule for building Node.js native addons (.node files).
"""

load("//bzl/js:js.bzl", "js_files_provider")

def _node_addon_impl(ctx):
    binary_out = ctx.actions.declare_file("{}.node".format(ctx.label.name))
    js_out = ctx.actions.declare_file("{}.js".format(ctx.label.name))

    ctx.actions.run_shell(
        outputs = [binary_out],
        inputs = [ctx.file.src],
        mnemonic = "NodeAddonCopy",
        command = "cp {src} {dst}".format(src = ctx.file.src.path, dst = binary_out.path),
    )

    ctx.actions.write(
        output = js_out,
        content = """const path = require('path')

try {{
  const appleFrameworkPath = process.env.APPLE_FRAMEWORK_PATH
  if (appleFrameworkPath) {{
    process.dlopen(module, path.join(appleFrameworkPath, '{framework_name}.framework', '{framework_name}'))
  }} else {{
    const runfilesDir = process.env.RUNFILES_DIR || path.join(__dirname, '{label}.runfiles')
    process.dlopen(module, path.join(runfilesDir, '{workspace}', '{addon}'))
  }}
}} catch (error) {{
  throw new Error(`node-addon:\\n${{error}}`)
}}
""".format(label = ctx.label.name, workspace = ctx.workspace_name, addon = binary_out.short_path, framework_name = ctx.label.name),
        is_executable = False,
    )

    return [
        DefaultInfo(files = depset([binary_out, js_out])),
        js_files_provider(transitive_srcs = depset([js_out]), runfiles = ctx.runfiles([binary_out]), includes = depset([ctx.bin_dir.path])),
    ]

_node_addon = rule(
    implementation = _node_addon_impl,
    attrs = {
        # A label that provides a dynamic library, usually a cc_binary with linkshared=1.
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
            providers = [CcInfo, DebugPackageInfo],
        ),
    },
)

def node_addon(name, visibility = ["//visibility:private"], **kwargs):
    """Creates a node addon. Parameters are the same as cc_binary.

    Args:
      name: The name of the node addon target.
      visibility: List of visibility labels for the node addon target.
      **kwargs: Additional keyword arguments passed to cc_binary.
    """
    shared_library_name = "{}-platform".format(name)
    kwargs["linkshared"] = 1  # This must be a shared library.

    native.cc_binary(
        name = shared_library_name,
        visibility = ["//visibility:private"],
        **kwargs
    )

    _node_addon(
        name = name,
        src = ":{}".format(shared_library_name),
        visibility = visibility,
    )
