load("//bzl/unity/impl:unity-upm-package.bzl", "TarballInfo")

def _export_manifest_implementation(ctx):
    upm_package_entries = ""
    for package in ctx.attr.upm_packages:
        upm_package_entries += """"%s": "file:../../../%s",
    """ % (package[TarballInfo].package_name, package[TarballInfo].tarball_file_name)

    plugin_entries = ""
    for plugin in ctx.attr.plugins:
        # We need to fail if we dont have the optional attr package_name in the plugin
        # since the manifest can't be properly made without it
        if not plugin.package_name:
            fail("plugin at %s doesnt have needed attribute package_name" % plugin.files_path_prefix)
        plugin_entries += """"%s": "file:../%s",
    """ % (plugin.package_name, plugin.project_relative_dir)

    export_manifest_out = ctx.actions.declare_file("%s/manifest.json" % (ctx.label.name))
    ctx.actions.expand_template(
        template = ctx.file.manifest_template,
        output = export_manifest_out,
        substitutions = {
            ctx.attr.template_symbol: upm_package_entries + plugin_entries + ctx.attr.static_content,
        },
    )

    return [
        DefaultInfo(files = depset([export_manifest_out])),
    ]

export_manifest = rule(
    implementation = _export_manifest_implementation,
    attrs = {
        "manifest_template": attr.label(
            default = Label("//bzl/unity:manifest-template.json"),
            allow_single_file = True,
        ),
        "static_content": attr.string(
            default = "",
        ),
        "template_symbol": attr.string(
            default = "%CONTENT%",
        ),
        "upm_packages": attr.label_list(
            default = [],
        ),
        "plugins": attr.label_list(
            default = [],
        ),
    },
)
