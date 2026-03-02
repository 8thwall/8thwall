load("@bazel_skylib//rules:common_settings.bzl", "BuildSettingInfo")
load("//bzl/unity/impl:unity-plugin.bzl", "copy_action")

TarballInfo = provider(
    "Info of the made upm tarball",
    fields = {
        "package_name": "name unity will recognize this upm as when imported",
        "tarball_file_name": "name of the tarball file",
        "debugging_symbols": "any debugging_symbol files of libs present in the upm package",
    },
)

def _unity_upm_package_impl(ctx):
    if not ctx.attr.plugin.package_name:
        fail("plugin at %s doesnt have needed attribute package_name" % ctx.attr.plugin.files_path_prefix)
    debugging_symbols = []
    if ctx.attr.plugin.debugging_symbol_files:
        for platform, files in ctx.attr.plugin.debugging_symbol_files.items():
            for file in files.to_list():
                debugging_symbol_path = "debugging-symbols/{}/{}/{}".format(ctx.attr.plugin.label.name, platform, file.basename)
                debugging_symbol_out = ctx.actions.declare_file(debugging_symbol_path)
                copy_action(ctx, file, debugging_symbol_out)
                debugging_symbols.append(debugging_symbol_out)

    tarballPath = "{}-{}.tgz".format(ctx.attr.plugin.package_name, ctx.attr.version[BuildSettingInfo].value)
    tarball = ctx.actions.declare_file(tarballPath)
    ctx.actions.run(
        inputs = ctx.attr.plugin.outputs + [ctx.executable._npm_bin],
        outputs = [tarball],
        executable = ctx.executable._generate_upm_package,
        env = {
            "WORKSPACE": ctx.workspace_name,
            "OUTPUT": tarball.path,
            "DIR": ctx.attr.plugin.files_path_prefix,
            "PACKING_DIR": ctx.label.name,
            "NPM_BIN": ctx.executable._npm_bin.path,
        },
        arguments = [d.path for d in ctx.attr.plugin.outputs],
    )

    return [
        # We need to add the debugging_symbols to the DefaultInof files as we need to be able to grab the
        # debugging_symbols when we just build the upm targets in ci to upload to artifactory
        DefaultInfo(files = depset([tarball])),
        OutputGroupInfo(lib_debugging_symbols = depset(debugging_symbols)),
        TarballInfo(
            package_name = ctx.attr.plugin.package_name,
            tarball_file_name = tarballPath,
            debugging_symbols = depset(debugging_symbols),
        ),
    ]

unity_upm_package = rule(
    implementation = _unity_upm_package_impl,
    attrs = {
        "plugin": attr.label(
            mandatory = True,
        ),
        "_generate_upm_package": attr.label(
            default = "//bzl/unity:generate-upm-package",
            cfg = "exec",
            executable = True,
        ),
        "_npm_bin": attr.label(
            default = "//bzl/unity/impl:npm_bin",
            allow_single_file = True,
            cfg = "exec",
            executable = True,
        ),
        "version": attr.label(
            mandatory = True,
            providers = [BuildSettingInfo],
        ),
    },
)
